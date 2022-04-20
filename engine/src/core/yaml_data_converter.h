#ifndef INCLUDED_YAML_DATA_CONVERTER
#define INCLUDED_YAML_DATA_CONVERTER

#include <unordered_map>
#include <functional>
#include <string>
#include <yaml-cpp/yaml.h>
using namespace std;

#include "core/os.h"
#include "common/utils.h"

// Forward declarations
class SceneTree;
class SpriteAnimationSet;

class YAMLDataConverter {

    private:
        template<typename ValueType>
        static ValueType convertToGenericType(YAML::Node data, string type_name, string error_prefix) {
            try {
                return data.as<ValueType>();
            }
            catch (YAML::TypedBadConversion<ValueType>) {
                ASSERT_MSG(false, "Could not convert data to generic type '" + string(typeid(ValueType).name()) + "'");
            }
            return (ValueType)NULL;
        }
        static Vector2 convertToVector2(YAML::Node data, string type_name, string error_prefix);
        static shared_ptr<SpriteAnimationSet> convertToSpriteAnimationSet(YAML::Node data, string type_name, string error_prefix);
    public:

        enum class DATA_TYPE {
            VALUE, ARRAY, MAP, NONE
        };

        static DATA_TYPE getYAMLDataType(YAML::Node data) {
            if (data.IsScalar()) return DATA_TYPE::VALUE;
            if (data.IsSequence()) return DATA_TYPE::ARRAY;
            if (data.IsMap()) return DATA_TYPE::MAP;
            return DATA_TYPE::NONE;
        }

        static string dataType2String(DATA_TYPE type) {
            switch (type) {
                case DATA_TYPE::VALUE: return "value";
                case DATA_TYPE::ARRAY: return "array";
                case DATA_TYPE::MAP: return "dictionary";
                default: return "none";
            }
        }

        YAMLDataConverter() {
            registerTypeConverter<string>(convertToGenericType<string>, "string", {DATA_TYPE::VALUE});
            registerTypeConverter<bool>(convertToGenericType<bool>, "bool", {DATA_TYPE::VALUE});
            registerTypeConverter<int>(convertToGenericType<int>, "int", {DATA_TYPE::VALUE});
            registerTypeConverter<float>(convertToGenericType<float>, "float", {DATA_TYPE::VALUE});

            registerTypeConverter<Vector2>(convertToVector2, "Vector2", {DATA_TYPE::ARRAY});
            registerTypeConverter<shared_ptr<SpriteAnimationSet>>(convertToSpriteAnimationSet, "SpriteAnimationSet", {DATA_TYPE::VALUE, DATA_TYPE::ARRAY});
        }

        template<typename ValueType>
        ValueType convertData(YAML::Node data) {
            ASSERT_MSG(isTypeConverterRegistered<ValueType>(), "Type '" + string(typeid(ValueType).name()) + "' has no registered converter");
            return ((Converter<ValueType>*)converters[typeid(ValueType).name()])->convertData(data);
        }

        template<typename ValueType>
        void registerTypeConverter(function<ValueType(YAML::Node, string, string)> converter_method, string type_name, vector<DATA_TYPE> allowed_data_types) {
            ASSERT_MSG(!isTypeConverterRegistered<ValueType>(), "Type '" + string(typeid(ValueType).name()) + "' with name '" + type_name + "' already has a registered converter")
            converters[typeid(ValueType).name()] = new Converter<ValueType>(converter_method, type_name, allowed_data_types);
        }

        template<typename ValueType>
        bool isTypeConverterRegistered() {
            return converters.count(typeid(ValueType).name());
        }

    private:
        class ConverterBase {};
        template<typename ValueType>
        class Converter: public ConverterBase {
            private:
                function<ValueType(YAML::Node, string, string)> method;
                string name;
                vector<DATA_TYPE> allowed_types;
            public:
                Converter(function<ValueType(YAML::Node, string, string)> converter_method, string type_name, vector<DATA_TYPE> allowed_data_types) {
                    method = converter_method;
                    name = type_name;
                    allowed_types = allowed_data_types;
                }
                ValueType convertData(YAML::Node data) {
                    string error_prefix = "An error occurred while converting YAML data to type '" + name + "': ";
                    
                    // Get the data type of the passed data
                    DATA_TYPE data_type = getYAMLDataType(data);

                    if (!vectorContainsValue(allowed_types, data_type)) {

                        // If a single value is allowed, and the data is an array with 1 value, use that value instead
                        if (data_type == DATA_TYPE::ARRAY && data.size() == 1 && vectorContainsValue(allowed_types, DATA_TYPE::VALUE)) {
                            data = data[0];
                        }
                        #if DEBUG_ENABLED
                        else {
                            string error_message = "Data type must be one of the following: (";
                            for (DATA_TYPE allowed : allowed_types) {
                                error_message += " " + dataType2String(allowed);
                            }
                            ASSERT_MSG(false, error_prefix + error_message + " ), but is of type " + dataType2String(data_type));
                        }
                        #endif
                    }

                    // Call the converter method with the passed data and return result
                    return method(data, name, error_prefix);
                }
        };
        
        unordered_map<string, ConverterBase*> converters;

};

#endif
#ifndef INCLUDED_OBJECT_CONSTRUCTOR
#define INCLUDED_OBJECT_CONSTRUCTOR

#include <iostream>
#include <string>
#include <unordered_map>
// #include <yaml-cpp/yaml.h>
#include <type_traits>
using namespace std;

#include "common/utils.h"
#include "engine/src/core/yaml_data_converter.h"

// Forward declarations
class SceneTree;

class ObjectConstructorBase {
    public:
        template<typename InheritedType>
        bool inheritsType() { return false; }
};

template<typename ObjectType>
class ObjectConstructor: public ObjectConstructorBase {

    private:
        class SetterBase {
            // public:
                // virtual void set(ObjectType* object, YAML::Node data, YAMLDataConverter* converter) {}
        };

        // Stores the setter method for this object type
        template<typename PropertyType>
        class Setter: public SetterBase {
            public:
                void (ObjectType::*setter_method)(PropertyType);

                Setter(void (ObjectType::*method)(PropertyType)) {
                    setter_method = method;
                }

                void set(ObjectType* object, PropertyType value) {
                    (object->*setter_method)(value);
                }

                // void set(ObjectType* object, YAML::Node data, YAMLDataConverter* converter) {
                //     if (!converter->isTypeConverterRegistered<PropertyType>()) {
                //         warn("Cannot set object value, type has no registered YAML data converter", true);
                //         return;
                //     }
                //     (object->*setter_method)(converter->convertData<PropertyType>(data));
                // }

        };

        class ConstructorBase {};

        template<typename... Args>
        class Constructor: public ConstructorBase { 
            private:
                // ObjectType may change later, so it is stored on initialisation as a lambda that returns an instance using this class
                function<ObjectType*(Args...)> method;

            public:
                Constructor(function<ObjectType*(Args...)> constructor_method) { method = constructor_method; }
                ObjectType* getNewInstance(Args... args) { return method(args...); }
        };

        YAMLDataConverter* converter;
        ConstructorBase* constructor;

    public:

        ObjectConstructor(YAMLDataConverter* data_converter) {
            converter = data_converter;
        }

        template<typename... ConstructorArgs>
        void init() {
            constructor = new Constructor<ConstructorArgs...>([](ConstructorArgs... args){return new ObjectType(args...);});
        }

        unordered_map<string, SetterBase*> setters;

        template<typename PropertyType>
        ObjectConstructor<ObjectType>* registerProperty(string property_name, void (ObjectType::*setter)(PropertyType)) {
            if (isPropertyRegistered(property_name)) {
                warn("Property '" + property_name + "' is already registered", true);
                return this;
            }
            setters[property_name] = new Setter<PropertyType>(setter);
            return this;
        }
        
        template<typename PropertyType>
        void setProperty(ObjectType* object, string property_name, PropertyType value) {
            if (!isPropertyRegistered(property_name)) {
                warn("Property '" + property_name + "' is not registered", true);
                return;
            }
            ((Setter<PropertyType>*)setters[property_name])->set(object, value);
        }

        // void setProperty(ObjectType* object, string property_name, YAML::Node data) {
        //     if (!isPropertyRegistered(property_name)) {
        //         warn("Property '" + property_name + "' is not registered", true);
        //         return;
        //     }
        //     setters[property_name]->set(object, data, converter);
        // }

        bool isPropertyRegistered(string property_name) {
            return setters.count(property_name);
        }

        template<typename... ConstructorArgs>
        ObjectType* createInstance(ConstructorArgs... constructor_args) {
            return ((Constructor<ConstructorArgs...>*)constructor)->getNewInstance(constructor_args...);
        }

        template<typename InheritedType>
        bool inheritsType() {
            return is_base_of<InheritedType, ObjectType>::value;
        }


};

#endif
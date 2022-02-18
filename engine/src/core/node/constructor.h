#ifndef INCLUDED_NODE_CONSTRUCTOR
#define INCLUDED_NODE_CONSTRUCTOR

#include <iostream>
#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <type_traits>
using namespace std;

#include "icecream.hpp"

#include "engine/src/core/node/node.h"
#include "engine/src/utils.h"

// Forward declarations
class NodeManager;

class ObjectConstructorBase {

};

class YAMLTypeConverter {

    public:
        
// unordered_map<string, vector<string>> conversion_data_types = {
//     {"string", {"single value"}},
//     {"bool", {"single value"}},
//     {"int", {"single value"}},
//     {"float", {"single value"}},
//     {"Vector2", {"array"}},
//     {"SpriteAnimationSet", {"single value", "array"}}
// };

        enum YAML_FORMAT {
            VALUE, ARRAY, MAP, NONE
        };

        const unordered_map<Node::PROPERTY_TYPE, vector<YAML_FORMAT>> property_type_formats = {
            {Node::PROPERTY_TYPE::STRING, {VALUE}}
        };

};

template<typename ObjectType>
class ObjectConstructor: public ObjectConstructorBase {

    private:

        class SetterBase {};

        template<typename PropertyType>
        class Setter: public SetterBase { // Stores the setter method for this object type
            public:
                void (ObjectType::*setter_method)(PropertyType);

                Setter(void (ObjectType::*method)(PropertyType)) {
                    setter_method = method;
                }

                void set(ObjectType* object, PropertyType value) {
                    (object->*setter_method)(value);
                }
        };

    public:

        unordered_map<string, SetterBase*> setters;

        template<typename PropertyType>
        ObjectConstructor<ObjectType>* registerProperty(string property_name, void (ObjectType::*setter)(PropertyType)) {
            if (setters.count(property_name)) {
                warn("Property '" + property_name + "' is already registered", true);
                return this;
            }
            setters[property_name] = new Setter<PropertyType>(setter);
            return this;
        }
        
        template<typename PropertyType>
        void setProperty(ObjectType* object, string property_name, PropertyType value) {
            if (!setters.count(property_name)) {
                warn("Property '" + property_name + "' is not registered", true);
                return;
            }
            ((Setter<PropertyType>*)setters[property_name])->set(object, value);
        }

        template<typename... ConstructorArgs>
        ObjectType* createInstance(ConstructorArgs... constructor_args) {
            return new ObjectType(constructor_args...);
        }

        template<typename InheritedType>
        bool inheritsType() {
            return is_base_of<InheritedType, ObjectType>::value;
        }

};

class NodePropertyTypeConveterBase {
    public:
        template<typename PropertyType = void>
        PropertyType convertData() { return NULL; }
};

template<typename PropertyType>
class NodePropertyTypeConverter: public NodePropertyTypeConveterBase {
    public:
        NodePropertyTypeConverter(function<PropertyType(YAML::Node)> converter_method) { method = converter_method; }
        PropertyType convertData(YAML::Node data) {
            return method(data);
        }
    private:
        function<PropertyType(YAML::Node)> method;
};


#endif
#ifndef INCLUDED_OBJECT_CONSTRUCTOR
#define INCLUDED_OBJECT_CONSTRUCTOR

#include <iostream>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <tuple>
// using namespace std;

#include "common/utils.h"
#include "engine/src/core/yaml_data_converter.h"

// Forward declarations
class SceneTree;

class ObjectConstructorBase {
    public:
        virtual bool inheritsNode() { return false; }
};

template<typename ObjectType>
class ObjectConstructor: public ObjectConstructorBase {

    private:
        struct Operation {
            enum TYPE { SETTER, METHOD };
            TYPE type;
            Operation(TYPE _type) { type = _type; }
            virtual void use(ObjectType* object, YAML::Node data, YAMLDataConverter* converter) = 0;
        };

        // Stores the setter method for this object type
        template<typename PropertyType>
        struct Setter: public Operation {
            void (ObjectType::*setter_method)(PropertyType);

            Setter(void (ObjectType::*method)(PropertyType)): Operation(Operation::SETTER) {
                setter_method = method;
            }

            void use(ObjectType* object, PropertyType value) {
                (object->*setter_method)(value);
            }

            void use(ObjectType* object, YAML::Node data, YAMLDataConverter* converter) {
                ASSERT_MSG(converter->isTypeConverterRegistered<PropertyType>(), "Type must have a registered YAML data converter");
                (object->*setter_method)(converter->convertData<PropertyType>(data));
            }
        };

        template<typename ReturnType, typename... Arguments>
        struct Method: public Operation {
            ReturnType (ObjectType::*method)(Arguments...);

            Method(ReturnType (ObjectType::*_method)(Arguments...)): Operation(Operation::METHOD) {
                method = _method;
            }

            void use(ObjectType* object, Arguments... args) {
                (object->*method)(args...);
            }

            void use(ObjectType* object, YAML::Node data, YAMLDataConverter* converter) {
                ASSERT(data.IsSequence() || (data.IsNull() && TYPE_COUNT(Arguments) == 0));
                ASSERT(data.size() == TYPE_COUNT(Arguments));

                tuple<Arguments...> args;
                parseArguments<0, Arguments...>(args, data, converter);

                call(object, args, make_index_sequence<TYPE_COUNT(Arguments)>{});
            }

            private:

                template<size_t... I>
                void call(ObjectType* object, tuple<Arguments...>& args, index_sequence<I...>) {
                    (object->*method)(get<I>(args)...);
                }

                template<size_t index, typename Type, typename... Tail>
                void parseArguments(tuple<Arguments...>& tuple, YAML::Node data, YAMLDataConverter* converter) {
                    ASSERT(converter->isTypeConverterRegistered<Type>());
                    get<index>(tuple) = converter->convertData<Type>(data[index]);
                    if (TYPE_COUNT(Tail) > 0) {
                        parseArguments<index + 1, Tail...>(tuple, data, converter);
                    }
                }

                template<size_t index>
                void parseArguments(tuple<Arguments...> tuple, YAML::Node data, YAMLDataConverter* converter) {}

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

        unordered_map<string, Operation*> operations;

        template<typename ReturnType, typename... Arguments>
        ReturnType (ObjectType::*getMethod(string name))(Arguments...) {
            Method<ReturnType, Arguments...>* method = (Method<ReturnType, Arguments...>*)operations[name];
            return method->method;
        }

        template<typename PropertyType>
        ObjectConstructor<ObjectType>* registerProperty(string property_name, void (ObjectType::*setter)(PropertyType)) {
            ASSERT_MSG(!isMethodRegistered(property_name), "'" + property_name + "' is already registered as a method");
            ASSERT_MSG(!isPropertyRegistered(property_name), "'" + property_name + "' is already registered as a property");
            operations[property_name] = new Setter<PropertyType>(setter);
            return this;
        }

        template<typename... Arguments, typename ReturnType = void>
        ObjectConstructor<ObjectType>* registerMethod(string method_name, ReturnType (ObjectType::*method)(Arguments...)) {
            ASSERT_MSG(!isPropertyRegistered(method_name), "'" + method_name + "' is already registered as a property");   
            ASSERT_MSG(!isMethodRegistered(method_name), "'" + method_name + "' is already registered as a method");
            operations[method_name] = new Method<ReturnType, Arguments...>(method);
            return this;
        }

        template<typename PropertyType>
        void setProperty(ObjectType* object, string property_name, PropertyType value) {
            ASSERT_MSG(isPropertyRegistered(property_name), "Property '" + property_name + "' is not registered");
            ((Setter<PropertyType>*)operations[property_name])->use(object, value);
        }

        void setProperty(ObjectType* object, string property_name, YAML::Node data) {
            ASSERT_MSG(isPropertyRegistered(property_name), "Property '" + property_name + "' is not registered");
            operations[property_name]->use(object, data, converter);
        }

        template<typename... Arguments>
        void callMethod(ObjectType* object, string method_name, Arguments... args) {
            ASSERT(isMethodRegistered(method_name));
            ((Method<Arguments...>)operations[method_name])->use(object, args...);
        }

        void callMethod(ObjectType* object, string method_name, YAML::Node data) {
            ASSERT(isMethodRegistered(method_name));
            operations[method_name]->use(object, data, converter);
        }

        bool isPropertyRegistered(string property_name) {
            auto operation = operations.find(property_name);
            if (operation == operations.end()) {
                return false;
            }
            return operation->second->type == Operation::SETTER;
        }

        bool isMethodRegistered(string method_name) {
            auto operation = operations.find(method_name);
            if (operation == operations.end()) {
                return false;
            }
            return operation->second->type == Operation::METHOD;
        }

        template<typename... ConstructorArgs>
        ObjectType* createInstance(ConstructorArgs... constructor_args) {
            return ((Constructor<ConstructorArgs...>*)constructor)->getNewInstance(constructor_args...);
        }

        bool inheritsNode() {
            return is_base_of<Node, ObjectType>::value;
        }


};

#endif
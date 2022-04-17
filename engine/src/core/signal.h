#ifndef INCLUDED_SIGNAL
#define INCLUDED_SIGNAL

#include "core/os.h"

#include <functional>
#include <iostream>
#include <unordered_map>
#include <any>
#include <list>
using namespace std;

// Forward declarations
class Node;
class Engine;

template<typename... CallbackArgs>
class Signal {
    public:

        ~Signal() {
            for (BaseConnectionHolder* holder : connections) {
                delete holder;
            }
        }

        class BaseConnection;

        void emit(CallbackArgs... arguments) {
            for (BaseConnectionHolder* holder : connections) {
                auto conn = holder->connections.begin();
                while (conn != holder->connections.end()) {
                    (*conn)->call(arguments...);
                    if ((*conn)->isOneShot()) {
                        delete *conn;
                        holder->connections.erase(conn++);
                    }
                    else {
                        conn++;
                    }
                }
            }
            onEmission();
        }

        template<typename ObjectType, typename... BindArguments, typename MethodReturnType = void>
        void connect(MethodReturnType (ObjectType::*callback)(CallbackArgs..., BindArguments...), ObjectType* object, bool one_shot = false, BindArguments... binds) {

            Connection<ObjectType, MethodReturnType, BindArguments...>* connection = new Connection<ObjectType, MethodReturnType, BindArguments...>(one_shot, binds...);
            connection->setup(callback, object);

            ConnectionHolder<ObjectType>* connection_holder = NULL;
            for (BaseConnectionHolder* holder : connections) {
                if (holder->matches(object)) {
                    connection_holder = (ConnectionHolder<ObjectType>*)holder;
                    break;
                }
            }

            if (connection_holder == NULL) {
                connection_holder = new ConnectionHolder(object);
                connections.push_back(connection_holder);
            }

            connection_holder->connections.push_back(connection);
        }

        template<typename ObjectType, typename... BindArguments, typename MethodReturnType = void>
        void connectWithoutArgs(MethodReturnType (ObjectType::*callback)(BindArguments...), ObjectType* object, bool one_shot = false, BindArguments... binds) {

            Connection<ObjectType, MethodReturnType, BindArguments...>* connection = new Connection<ObjectType, MethodReturnType, BindArguments...>(one_shot, binds...);
            connection->setupWithoutArgs(callback, object);

            ConnectionHolder<ObjectType>* connection_holder = NULL;
            for (BaseConnectionHolder* holder : connections) {
                if (holder->matches(object)) {
                    connection_holder = (ConnectionHolder<ObjectType>*)holder;
                    break;
                }
            }

            if (connection_holder == NULL) {
                connection_holder = new ConnectionHolder<ObjectType>(object);
                connections.push_back(connection_holder);
            }

            connection_holder->connections.push_back(connection);
        }

        template<typename ObjectType>
        void disconnect(ObjectType* object) {
            for (auto holder = connections.begin(); holder != connections.end(); holder++) {
                if ((*holder)->matches(object)) {
                    delete *holder;
                    connections.erase(holder);
                    break;
                }
            }
        }

        void disconnectAll() {
            for (BaseConnectionHolder* holder : connections) {
                delete holder;
            }
            connections.clear();
        }

        int getConnectionCount() {
            int total = 0;
            for (BaseConnectionHolder* holder : connections) {
                total += holder->connections.size();
            }
            return total;
        }

        template<typename ObjectType>
        int getConnectionCount(ObjectType* object) {
            for (BaseConnectionHolder* holder : connections) {
                if (holder->matches(object)) {
                    return holder->connections.size();
                }
            }
            return 0;
        }

        template<typename ObjectType>
        vector<BaseConnection*>* getConnections(ObjectType* object) {
            for (BaseConnectionHolder* holder : connections) {
                if (holder->matches(object)) {
                    return &holder->connections;
                }
            }
            return NULL;
        }
        
        // unordered_map<any, vector<BaseConnection*>>* getAllConnections() {
        //     return &connections;
        // }

        class BaseConnection {
            public:
                virtual void call(CallbackArgs... args) = 0;
                virtual bool isOneShot() = 0;
        };

        template<typename ObjectType, typename MethodReturnType, typename... BindArguments>
        class Connection: public BaseConnection {
            private:
                ObjectType* object;
                MethodReturnType (ObjectType::*callback)(CallbackArgs..., BindArguments...);
                MethodReturnType (ObjectType::*callback_noargs)(BindArguments...);
                tuple<BindArguments...> binds;
                bool one_shot;

                bool ignore_arguments = false;

                template<size_t ...I>
                void callWithBinds(CallbackArgs... arguments, index_sequence<I...>) {
                    if (!ignore_arguments) {
                        (object->*callback)(arguments..., get<I>(binds)...);
                    }
                    else {
                        (object->*callback_noargs)(get<I>(binds)...);
                    }
                }

            public:
                Connection(bool one_shot, BindArguments... bind_args)
                    :binds(forward<BindArguments>(bind_args)...) {
                        this->one_shot = one_shot;
                    }

                void setup(MethodReturnType (ObjectType::*callback_function)(CallbackArgs..., BindArguments...), ObjectType* function_object) {
                    object = function_object;
                    callback = callback_function;
                    ignore_arguments = false;
                }

                void setupWithoutArgs(MethodReturnType (ObjectType::*callback_function)(BindArguments...), ObjectType* function_object) {
                    object = function_object;
                    callback_noargs = callback_function;
                    ignore_arguments = true;
                }
                
                void call(CallbackArgs... arguments) {
                    static constexpr auto size = tuple_size<tuple<BindArguments...>>::value;
                    callWithBinds(arguments..., make_index_sequence<size>{});
                }

                ObjectType* getObject() {
                    return object;
                }

                bool isOneShot() {
                    return one_shot;
                }
        };

    private:
        void onEmission() { last_emission_time = OS::getTime(); }

        double last_emission_time = -1;

        struct BaseConnectionHolder {
            virtual bool matches(any match_object) = 0;
            list<BaseConnection*> connections;
            ~BaseConnectionHolder() {
                for (BaseConnection* connection : connections) {
                    delete connection;
                }
            }
        };

        template<typename ObjectType>
        struct ConnectionHolder: BaseConnectionHolder {

            ConnectionHolder(ObjectType* _object) {
                object = _object;
            }
            
            ObjectType* object;

            bool matches(any match_object) {
                if (match_object.type() != typeid(ObjectType*)) {
                    return false;
                }
                return any_cast<ObjectType*>(match_object) == object;
            }
        };
        vector<BaseConnectionHolder*> connections;
};

#endif
#ifndef INCLUDED_SIGNAL
#define INCLUDED_SIGNAL

#include <raylib-cpp.hpp>
#include <functional>
#include <iostream>
#include <unordered_map>
using namespace std;

#include <icecream.hpp> // Debug

// Forward declarations
class Node;
class Engine;

template<typename CallbackReturnType = void, typename... CallbackArgs>
class Signal {
    public:
        void await(Engine* engine);

        void emit(CallbackArgs... arguments) {

            for (auto pair : callbacks) {
                for (auto i : pair.second) {
                    i(arguments...);
                }
            }
            
            for (auto pair : member_callbacks) {
                for (auto i : pair.second) {
                    i->call(arguments...);
                }
            }

            onEmission();
        }

        void connect(function<CallbackReturnType(CallbackArgs...)> callback, string tag = "") {
            if (callbacks.count(tag)) {
                callbacks[tag].push_back(callback);
            }
            else {
                callbacks[tag] = vector<function<CallbackReturnType(CallbackArgs...)>>{callback};
            }
        }
        template<typename ObjectType>
        void connect(CallbackReturnType (ObjectType::*callback)(CallbackArgs...), ObjectType* object, string tag = "") {
            if (member_callbacks.count(tag)) {
                member_callbacks[tag].push_back(make_shared<MemberCallback<ObjectType>>(callback, object));
            }
            else {
                member_callbacks[tag] = vector<shared_ptr<MemberCallbackBase>>{make_shared<MemberCallback<ObjectType>>(callback, object)};
            }
        }

        void disconnect(string tag) {
            callbacks.erase(tag);

            if (member_callbacks.count(tag)) {
                for (auto i : member_callbacks[tag]) {
                    delete i;
                }
                member_callbacks.erase(tag);
            }
        }

        void disconnectAll() {
            callbacks.clear();
            for (auto pair : member_callbacks) {
                for (auto i : pair.second) {
                    delete i;
                }
            }
            member_callbacks.clear();
        }

        // Why doesn't this work when defined in signal.cpp ?!
        bool connected() {
            return callbacks.size() > 0 || member_callbacks.size() > 0;
        }

    protected:
        void onEmission() { last_emission_time = GetTime(); }
        class MemberCallbackBase {
            public:
                virtual void call(CallbackArgs... args) {}
        };

        template<typename ObjectType>
        class MemberCallback: public MemberCallbackBase {
            private:
                ObjectType* object;
                CallbackReturnType (ObjectType::*callback)(CallbackArgs...);
            public:
                MemberCallback(CallbackReturnType (ObjectType::*callback_function)(CallbackArgs...), ObjectType* function_object) {
                    object = function_object;
                    callback = callback_function;
                }
                void call(CallbackArgs... arguments) {
                    (object->*callback)(arguments...);
                }
        };

        unordered_map<string, vector<function<CallbackReturnType(CallbackArgs...)>>> callbacks;
        unordered_map<string, vector<shared_ptr<MemberCallbackBase>>> member_callbacks;
        double last_emission_time = -1;
};

#endif
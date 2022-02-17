#include <raylib-cpp.hpp>
#include <functional>
#include <iostream>
using namespace std;

#include <icecream.hpp> // Debug

// Forward declarations
class Node;
class NodeManager;

template<typename CallbackReturnType = void, typename... CallbackArgs>
class Signal {
    public:
        void await(NodeManager* manager);

        void emit(CallbackArgs... arguments) {
            for (auto i = callbacks.begin(); i != callbacks.end(); ++i) {
                (*i)(arguments...);
            }
            for (auto i = member_callbacks.begin(); i != member_callbacks.end(); ++i) {
                (*i)->call(arguments...);
            }
            onEmission();
        }

        void connect(function<CallbackReturnType(CallbackArgs...)> callback) {
            callbacks.push_back(callback); 
        }
        template<typename ObjectType>
        void connect(CallbackReturnType (ObjectType::*callback)(CallbackArgs...), ObjectType* object) {
            member_callbacks.push_back(new MemberCallback<ObjectType>(callback, object));
        }

        // Why doesn't this work when defined in signal.cpp ?!
        bool connected() {
            return callbacks.size() > 0 || member_callbacks.size() > 0;
        }

    protected:
        void onEmission() { last_emission_time = GetTime(); }
    private:

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

        vector<MemberCallbackBase*> member_callbacks;
        vector<function<CallbackReturnType(CallbackArgs...)>> callbacks;
        double last_emission_time = -1;
};

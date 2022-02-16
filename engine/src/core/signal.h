#ifndef SIGNAL_INCLUDED
#define SIGNAL_INCLUDED

using namespace std;
#include <raylib-cpp.hpp>
#include <functional>
#include <iostream>

#include <icecream.hpp> // Debug

// Forward declarations
class Node;

class Signal {
    private:
        vector<tuple<function<void(void*)>, void*>> callbacks;
        double last_emission_time = -1;
    protected:
        void onEmission() { last_emission_time = GetTime(); }
    public:
        void await() {
            double start_time = GetTime();
            while (start_time >= last_emission_time) {
                // Pass
            };
        }

        void emit() {
            for (auto i = callbacks.begin(); i != callbacks.end(); ++i) {
                get<0>(*i)(get<1>(*i));
            }
            onEmission();
        }
        void connect(function<void(void*)> callback, void* argument = NULL) { callbacks.push_back(make_tuple(callback, argument)); }
};

class NodeSignalStatic: public Signal {
    private:
        Node* caller;
        vector<tuple<function<void(void*, Node*)>, void*>> callbacks;
    public:
        NodeSignalStatic(Node* caller_node) { caller = caller_node; }
        void emit() {
            for (auto i = callbacks.begin(); i != callbacks.end(); ++i) {
                get<0>(*i)(get<1>(*i), caller);
                // Check if pointers are valid
            }
            onEmission();
        }
        void connect(function<void(void*, Node*)> callback, void* argument = NULL) { callbacks.push_back(make_tuple(callback, argument)); }
};

class NodeSignal: public Signal {
    private:
        vector<tuple<function<void(void*, Node*)>, void*>> callbacks;
    public:
        void emit(Node* node) {
            for (auto i = callbacks.begin(); i != callbacks.end(); ++i) {
                get<0>(*i)(get<1>(*i), node);
            }
            onEmission();
        }
        void connect(function<void(void*, Node*)> callback, void* argument = NULL) { callbacks.push_back(make_tuple(callback, argument)); }

};

#endif
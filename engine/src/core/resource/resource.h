#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

#include <raylib-cpp.hpp>
#include <vector>
#include <memory>
using namespace std;

// Forward declarations
class Node;
class Engine;
template<typename A, typename... B>
class Signal;

class Resource {
    public:
        Resource();
        ~Resource();
        
        virtual void process(float delta) {};
        unique_ptr<Signal<void, Resource*>> SIGNAL_DELETED = make_unique<Signal<void, Resource*>>();

        class ResourcePool {
            static string getName() {
                return "Resource";
            }
        };
};

#endif
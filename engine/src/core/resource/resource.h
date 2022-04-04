#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

#include "engine/src/raylib_include.h"
#include <vector>
#include <memory>
using namespace std;

#include "engine/src/core/signal.h"

// Forward declarations
class Node;
class Engine;

class Resource {
    public:
        Resource();
        ~Resource();
        
        virtual void process(float delta) {};
        Signal<> SIGNAL_DELETED;

        class ResourcePool {
            static string getName() {
                return "Resource";
            }
        };
};

#endif
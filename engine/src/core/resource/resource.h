#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

#include <vector>
#include <memory>
using namespace std;

#include "engine/src/core/signal.h"

// Forward declarations
class Node;
class Engine;

class Resource {
    public:
        ~Resource();

        virtual void process(float delta) {};
        Signal<> SIGNAL_DELETED;

    protected:
        Resource();
};

#endif
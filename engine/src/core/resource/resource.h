#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

#include <raylib-cpp.hpp>
#include <vector>
using namespace std;

// Forward declarations
class Node;
class Engine;
template<typename A, typename... B>
class Signal;

class Resource {
    public:
        Resource(Engine* engine_singleton);
        virtual void process(float delta) {};
    protected:
        Engine* engine;
    
};

#endif
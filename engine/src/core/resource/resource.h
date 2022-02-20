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
        Resource(Node* initial_linked_node);
        void linkNode(Node* node);
        void unlinkNode(Node* node);

        vector<Node*>* getLinkedNodes() { return &linked_nodes; }
        int getLinkedNodeCount() { return linked_nodes.size(); }

        Signal<void, Node*>* SIGNAL_NODE_LINKED;
        Signal<void, Node*>* SIGNAL_NODE_UNLINKED;
        Signal<void>* SIGNAL_DELETED;
    
        virtual void process(float delta) {};
    
    protected:
        Engine* engine;
    private:
        vector<Node*> linked_nodes;
    
};

#endif
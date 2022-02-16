#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

using namespace std;
#include <raylib-cpp.hpp>
#include <vector>

#include "engine/src/core/signal.h"

// Forward declarations
class Node;
class NodeManager;

class Resource {
    public:
        Resource(Node* initial_linked_node);
        void linkNode(Node* node);
        void unlinkNode(Node* node);

        std::vector<Node*>* getLinkedNodes() { return &linked_nodes; }
        int getLinkedNodeCount() { return linked_nodes.size(); }

        NodeSignal* SIGNAL_NODE_LINKED = new NodeSignal;
        NodeSignal* SIGNAL_NODE_UNLINKED = new NodeSignal;
        Signal* SIGNAL_DELETED = new Signal;
    
    protected:
        NodeManager* manager;
    
    private:
        vector<Node*> linked_nodes;
};

#endif
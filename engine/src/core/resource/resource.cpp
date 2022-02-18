#include "resource.h"

#include "engine/src/utils.h"
#include "engine/src/engine.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/core/signal.h"

Resource::Resource(Node* initial_linked_node) {

    SIGNAL_NODE_LINKED = new Signal<void, Node*>();
    SIGNAL_NODE_UNLINKED = new Signal<void, Node*>();
    SIGNAL_DELETED = new Signal<void>();
    
    linkNode(initial_linked_node);
    engine = initial_linked_node->getEngine();
    engine->resourceCreated(this);
}
void Resource::linkNode(Node* node) {
    if (!vectorContainsValue(&linked_nodes, node)) {
        linked_nodes.push_back(node);
        SIGNAL_NODE_LINKED->emit(node);

        node->SIGNAL_KILLED->connect<Resource>(&Resource::unlinkNode, this);
    }
}
void Resource::unlinkNode(Node* node) {
    vectorRemoveValue(&linked_nodes, node);

    if (linked_nodes.size() == 0) {
        SIGNAL_DELETED->emit();
        engine->resourceDeleted(this);
        engine->requestDeletion(this);
    }
    else {
        SIGNAL_NODE_UNLINKED->emit(node);
    }
}
#include "resource.h"

#include "engine/src/core/node/node.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/utils.h"

void linkedNodeKilled(void* self, Node* node) {
    ((Resource*)self)->unlinkNode(node);
}

Resource::Resource(Node* initial_linked_node) {
    linkNode(initial_linked_node);
    manager = initial_linked_node->getManager();
    manager->resourceCreated(this);
}
void Resource::linkNode(Node* node) {
    if (!vectorContainsValue(&linked_nodes, node)) {
        linked_nodes.push_back(node);
        SIGNAL_NODE_LINKED->emit(node);

        node->SIGNAL_KILLED->connect(linkedNodeKilled, this);
    }
}
void Resource::unlinkNode(Node* node) {
    vectorRemoveValue(&linked_nodes, node);

    if (linked_nodes.size() == 0) {
        SIGNAL_DELETED->emit();
        manager->resourceDeleted(this);
        manager->requestDeletion(this);
    }
    else {
        SIGNAL_NODE_UNLINKED->emit(node);
    }
}
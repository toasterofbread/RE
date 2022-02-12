#include "node.h"

class NodeManager: public NodeManagerParent {
    private:
        Node root_node;
    public:
        NodeManager();

        void processNodes(float delta);
        void addNode(Node* node);
        
        Node getRoot() {return root_node;}
        Node* getRootPointer() {return &root_node;}
};
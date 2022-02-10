#include "node_manager.h"
#include "utils.h"
#include "vector"

NodeManager::NodeManager() {
    root_node.init(&root_node, NULL);
}

void NodeManager::processNodes(float delta) {

    // print("NODE PROCESS");

    std::vector<Node*> nodes = root_node.getChildren();
    // print(nodes.size());

    for (auto i = nodes.cbegin(); i != nodes.cend(); ++i) {
        Node& node = **i;
        node.process(delta);
    }

}

void NodeManager::addNode(Node* node) {
    root_node.addChild(node);
}
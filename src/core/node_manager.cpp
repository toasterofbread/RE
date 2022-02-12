#include "node_manager.h"
#include "utils.h"
#include "vector"
#include "node.h"

#include "icecream.hpp"

int NodeManagerParent::getNewId() {
    current_id_max++;
    return current_id_max;
}

NodeManager::NodeManager() {
    root_node.init(&root_node, NULL, this);
}

void NodeManager::processNodes(float delta) {
    std::vector<Node*> nodes = root_node.getChildren();

    for (auto i = nodes.cbegin(); i != nodes.cend(); ++i) {
        Node& node = **i;
        node.process(delta);
    }
}

void NodeManager::addNode(Node* node) {
    root_node.addChild(node);
}

// - Resource loading / unloading -

Texture2D* NodeManagerParent::loadTexture(const char* file_path, int node_id) {

    const char* path = getResPath(file_path);
    Texture2D* ret;

    if (isTextureLoaded(file_path)) { // Texture is already loaded, just add the node to the userlist
        // ret = &LoadTexture(path);
        
    }
    else {
        loaded_textures.insert(std::make_pair(path, LoadTexture(path)));
        loaded_texture_users.insert(std::make_pair(path, std::vector<int>(node_id)));
    }

    return ret;


}

void NodeManagerParent::unloadTexture(Texture2D* texture, int node_id) {

}

bool NodeManagerParent::isTextureLoaded(const char* file_path) {
    return loaded_textures.count(getResPath(file_path)) > 0;
}
using namespace std;
#include "node_texture.h"

#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"

#include <functional>

NodeTexture::NodeTexture(Node* initial_linked_node, string autoload_file_path): Resource(initial_linked_node) {
    SIGNAL_NODE_LINKED->connect(onNodeLinked, this);
    SIGNAL_DELETED->connect(onDeleted, this);

    autoload_path = autoload_file_path;
    if (!isTextureLoaded() && hasAutoload()) {
        load(autoload_path);
    }
}

void NodeTexture::onNodeLinked(void* _self, Node* node) {
    NodeTexture* self = (NodeTexture*)_self;

    if (!self->isTextureLoaded() && self->hasAutoload()) {
        self->load(self->getAutoload());
    }
}

void NodeTexture::onDeleted(void* _self) {
    ((NodeTexture*)_self)->unload();
}

void NodeTexture::load(string file_path) {
    if (getLinkedNodeCount() == 0) {
        warn("Cannot load texture, no nodes are linked", true);
        return;
    }
    texture = manager->loadTexture(file_path, this);
    texture_loaded = true;
}

void NodeTexture::unload() {
    if (!isTextureLoaded()) {
        return;
    }
    manager->unloadTexture(texture, this);
}

Texture2D NodeTexture::getTexture() {
    if (!isTextureLoaded()) {
        warn("NodeTexture cannot provide texture as it is not loaded", true);
    }
    return texture;
}

int NodeTexture::getWidth() {
    if (!isTextureLoaded()) {
        warn("NodeTexture cannot provide texture width as texture is not loaded", true);
        return 0;
    }
    return getTexture().width;
}

int NodeTexture::getHeight() {
    if (!isTextureLoaded()) {
        warn("NodeTexture cannot provide texture height as texture is not loaded", true);
        return 0;
    }
    return getTexture().height;
}
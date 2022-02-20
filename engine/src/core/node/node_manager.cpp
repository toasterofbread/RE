#include "node_manager.h"

#include <vector>

#include "engine/src/utils.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node.h"
#include "engine/src/engine.h"
#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/input/input_event.h"

// Debug
#include "icecream.hpp"

NodeManager::NodeManager(Engine* engine_singleton) {
    engine = engine_singleton;

    engine->SIGNAL_OBJECT_TYPE_REGISTERED->connect<NodeManager>(&NodeManager::objectRegisteredAsConstructor, this);
}


void NodeManager::init() {
    root_node = new Node(engine);
    root_node->setName("Root");
}

void NodeManager::process(float delta) {
    for (auto i = root_node->getChildren()->begin(); i != root_node->getChildren()->end(); ++i) {
        (*i)->process(delta);
    }
}

void NodeManager::addNode(Node* node) {
    root_node->addChild(node);
}

int NodeManager::getNewNodeId() {
    current_id_max++;
    return current_id_max;
}

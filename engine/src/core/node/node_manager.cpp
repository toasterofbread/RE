#include "node_manager.h"

#include <vector>

#include "engine/src/utils.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node.h"
#include "engine/src/engine.h"

// Debug
#include "icecream.hpp"

SceneTree::SceneTree(Engine* engine_singleton) {
    engine = engine_singleton;

    engine->SIGNAL_OBJECT_TYPE_REGISTERED->connect<SceneTree>(&SceneTree::objectRegisteredAsConstructor, this);
}


void SceneTree::init() {
    root_node = new Node(engine);
    root_node->setName("Root");
    current_state = STATE::IDLE;
}

void SceneTree::process(float delta) {

    current_state = STATE::PROCESS;

    for (auto i = root_node->getChildren()->begin(); i != root_node->getChildren()->end(); ++i) {
        (*i)->process(delta);
    }
    
    current_state = STATE::POST_PROCESS;

    for (auto i = kill_queue.begin(); i != kill_queue.end(); ++i) {
        (*i)->kill();
        delete *i;
    }
    kill_queue.clear();

    current_state = STATE::IDLE;
}

void SceneTree::addNode(Node* node) {
    root_node->addChild(node);
}

void SceneTree::queueNodeKill(Node* node) {
    for (auto i = kill_queue.begin(); i != kill_queue.end(); ++i) {
        if ((*i)->isIndirectParentOf(node)) {
            return;
        }
    }
    kill_queue.push_back(node);
}

void SceneTree::onNodeCreated(Node* node) {
    global_node_count++;
}

void SceneTree::onNodeKilled(Node* node) {
    global_node_count--;
}

int SceneTree::getGlobalNodeCount() {
    return global_node_count;
}

int SceneTree::getNewNodeId() {
    current_id_max++;
    return current_id_max;
}

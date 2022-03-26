#include "scene_tree.h"

#include <vector>

#include "engine/src/utils.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/node_types/node_2d.h"
#include "engine/src/engine.h"

// Debug
#include "icecream.hpp"

const int SceneTree::MIN_DRAW_LAYER;
const int SceneTree::MAX_DRAW_LAYER;

void SceneTree::init() {
    root_node = new Node;
    root_node->setName("Root");
    root_node->makeRoot(this);
    current_state = STATE::IDLE;
}

void SceneTree::process(float delta) {

    current_state = STATE::PROCESS;

    for (auto i = root_node->getChildren()->begin(); i != root_node->getChildren()->end(); ++i) {
        // !todo proper idle / physics frame implementation
        (*i)->process(delta);
        (*i)->physicsProcess(delta);
    }
    
    current_state = STATE::DRAW;

    for (int i = 0; i < MAX_DRAW_LAYER - MIN_DRAW_LAYER; i++) {
        for (auto node : drawable_nodes[i]) {
            node->draw();
        }
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

void SceneTree::onNodeAddedToTree(Node* node) {
    if (Node2D* node_2d = dynamic_cast<Node2D*>(node)) {
        drawable_nodes[node_2d->getDrawLayer(true) - MIN_DRAW_LAYER].push_back(node_2d);
        node_2d->SIGNAL_DRAW_LAYER_CHANGED.connect(&SceneTree::onDrawableNodeLayerChanged, this, "SceneTree");
    }
}

void SceneTree::onNodeRemovedFromTree(Node* node) {
    if (Node2D* node_2d = dynamic_cast<Node2D*>(node)) {
        vectorRemoveValue(&drawable_nodes[node_2d->getDrawLayer(true) - MIN_DRAW_LAYER], node_2d);
        node_2d->SIGNAL_DRAW_LAYER_CHANGED.disconnect("SceneTree");
    }
}

void SceneTree::onDrawableNodeLayerChanged(Node2D* node, int old_draw_layer) {
    vectorRemoveValue(&drawable_nodes[old_draw_layer - MIN_DRAW_LAYER], node);
    drawable_nodes[node->getDrawLayer(true) - MIN_DRAW_LAYER].push_back(node);
}
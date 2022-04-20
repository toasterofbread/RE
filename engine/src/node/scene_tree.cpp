#include "scene_tree.h"

#include <vector>

#include "common/utils.h"
#include "engine/src/core/signal.h"
#include "engine/src/node/node.h"
#include "engine/src/node/types/node_2d.h"
#include "engine/src/engine.h"
#include "engine/src/node/types/timer.h"
#include "node/types/camera_2d.h"

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

    current_state = STATE::KILL_NODES;

    for (auto i = kill_queue.begin(); i != kill_queue.end(); ++i) {
        Node* node = *i;
        node->kill();
        delete node;
    }
    kill_queue.clear();

    current_state = STATE::IDLE;
}

void SceneTree::addNode(Node* node) {
    ASSERT(!node->isInsideTree());
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
        drawable_nodes[node_2d->getGlobalDrawLayer() - MIN_DRAW_LAYER].push_back(node_2d);
        node_2d->SIGNAL_DRAW_LAYER_CHANGED.connect(&SceneTree::onDrawableNodeLayerChanged, this, false, node_2d);
    }
}

void SceneTree::onNodeRemovedFromTree(Node* node) {
    if (Node2D* node_2d = dynamic_cast<Node2D*>(node)) {
        vectorRemoveValue(&drawable_nodes[node_2d->getGlobalDrawLayer() - MIN_DRAW_LAYER], node_2d);
        node_2d->SIGNAL_DRAW_LAYER_CHANGED.disconnect(this);
    }
}

Timer* SceneTree::createTimer(float duration, bool free_on_timeout) {
    Timer* timer = new Timer;
    root_node->addChild(timer);

    if (free_on_timeout) {
        timer->SIGNAL_TIMEOUT.connect<Node>(&Timer::queueKill, timer);
    }

    timer->start(duration);
    return timer;
}

void SceneTree::setEnabledCamera2D(Camera2D* value) {
    enabled_camera_2d = value;
}

Camera2D* SceneTree::getEnabledCamera2D() {
    return enabled_camera_2d;
}

void SceneTree::setEnabledCamera3D(Camera3D* value) {
    enabled_camera_3d = value;
}

Camera3D* SceneTree::getEnabledCamera3D() {
    return enabled_camera_3d;
}

void SceneTree::onDrawableNodeLayerChanged(int old_draw_layer, int new_draw_layer, Node2D* node) {
    vectorRemoveValue(&drawable_nodes[old_draw_layer - MIN_DRAW_LAYER], node);
    drawable_nodes[new_draw_layer - MIN_DRAW_LAYER].push_back(node);
}
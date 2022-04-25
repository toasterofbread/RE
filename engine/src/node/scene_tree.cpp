#include "scene_tree.h"

#include <vector>

#include "common/utils.h"
#include "engine/src/core/signal.h"
#include "engine/src/node/node.h"
#include "engine/src/node/types/node_2d.h"
#include "engine/src/node/types/node_3d.h"
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

        for (auto drawable = drawable_nodes[i].begin(); drawable != drawable_nodes[i].end();) {
            (*drawable)->draw();

            if ((*drawable)->one_shot) {
                delete (*drawable);
                drawable = drawable_nodes[i].erase(drawable);
            }
            else {
                ++drawable;
            }
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
    if (Node2D* converted = dynamic_cast<Node2D*>(node)) {
        drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].push_back(new Drawable(converted));
        converted->SIGNAL_DRAW_LAYER_CHANGED.connect(&SceneTree::onDrawableNodeLayerChanged, this, false, converted);
        converted->SIGNAL_KILLED.connect(&SceneTree::onNodeRemovedFromTree, this, false, node);
    }
    else if (Node3D* converted = dynamic_cast<Node3D*>(node)) {
        drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].push_back(new Drawable(converted));
        converted->SIGNAL_DRAW_LAYER_CHANGED.connect(&SceneTree::onDrawableNodeLayerChanged, this, false, converted);
        converted->SIGNAL_KILLED.connect(&SceneTree::onNodeRemovedFromTree, this, false, node);
    }
}

void SceneTree::onNodeRemovedFromTree(Node* node) {
    if (Node2D* converted = dynamic_cast<Node2D*>(node)) {
        for (auto drawable = drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].begin(); drawable != drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].end(); drawable++) {
            if ((*drawable)->compare(converted)) {
                delete (*drawable);
                drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].erase(drawable);
                break;
            }
        }
        converted->SIGNAL_DRAW_LAYER_CHANGED.disconnect(this);
    }
    else if (Node3D* converted = dynamic_cast<Node3D*>(node)) {
        for (auto drawable = drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].begin(); drawable != drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].end(); drawable++) {
            if ((*drawable)->compare(converted)) {
                delete (*drawable);
                drawable_nodes[converted->getGlobalDrawLayer() - MIN_DRAW_LAYER].erase(drawable);
                break;
            }
        }
        converted->SIGNAL_DRAW_LAYER_CHANGED.disconnect(this);
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
    for (auto drawable = drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].begin(); drawable != drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].end(); drawable++) {
        if ((*drawable)->compare(node)) {
            drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].erase(drawable);
            drawable_nodes[new_draw_layer - MIN_DRAW_LAYER].push_back(*drawable);
            break;
        }
    }
}

void SceneTree::onDrawableNodeLayerChanged(int old_draw_layer, int new_draw_layer, Node3D* node) {
    for (auto drawable = drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].begin(); drawable != drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].end(); drawable++) {
        if ((*drawable)->compare(node)) {
            drawable_nodes[old_draw_layer - MIN_DRAW_LAYER].erase(drawable);
            drawable_nodes[new_draw_layer - MIN_DRAW_LAYER].push_back(*drawable);
            break;
        }
    }
}

void SceneTree::BaseDrawable::callNodeDraw() {
    if (type == NODE2D) {
        node_2d->draw();
    }
    else {
        node_3d->draw();
    }
}

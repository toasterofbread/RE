#include "node_2d.h"

void Node2D::draw() {
    if (show_gizmos) {
        string text = getTypeName();
        if (getName() != text) {
            text += " | " + getName();
        }
        if (!isVisible()) {
            text += " (invisible)";
        }
        
        // text += " | " + (string)int2char(getId());

        Vector2 global_position = getGlobalPosition(true);
        text += " | " + vector2str(global_position, 0);
        markPosition(global_position, text, BLACK);
    }
}

Vector2 Node2D::getPosition() {
    return position;
}
void Node2D::setPosition(Vector2 value) {
    position = value;
}

Vector2 Node2D::getGlobalPosition(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global position");
    }
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalPosition(suppress_warning) + getPosition();
    }

    return getPosition();
}
void Node2D::setGlobalPosition(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global position");
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        setPosition(value - parent_2d->getGlobalPosition(true));
    }
    else {
        setPosition(value);
    }
}

Vector2 Node2D::getScale() {
    return scale;
}
void Node2D::setScale(Vector2 value) {
    scale = value;
}

Vector2 Node2D::getGlobalScale(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global scale");
    }
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalScale(suppress_warning) * getScale();
    }

    return getScale();
}
void Node2D::setGlobalScale(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setScale(value / getGlobalScale(true));
}

float Node2D::getRotation() {
    return rotation;
}
void Node2D::setRotation(float value) {
    rotation = value;
}

float Node2D::getGlobalRotation(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global rotation");
    }
    
    if (getParent() == NULL) {
        return getRotation();
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalRotation(suppress_warning) + getRotation();
    }

    return getRotation();
}
void Node2D::setGlobalRotation(float value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setRotation(value - getGlobalRotation(true));
}

bool Node2D::isVisible(bool consider_parents) {
    
    if (!visible) {
        return false;
    }
    
    if (!consider_parents) {
        return visible;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->isVisible(true);
    }

    return visible;
}

void Node2D::setDrawLayer(int value) {
    int original_draw_layer = getDrawLayer(true);
    draw_layer = value; 
    SIGNAL_DRAW_LAYER_CHANGED.emit(this, original_draw_layer); 
}

int Node2D::getDrawLayer(bool consider_parents) {
    
    if (!consider_parents) {
        return draw_layer;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return clamp(parent_2d->getDrawLayer(true) + draw_layer, SceneTree::MIN_DRAW_LAYER, SceneTree::MAX_DRAW_LAYER);
    }

    return draw_layer;
}

void Node2D::onParentDrawLayerChanged(Node2D* _node, int old_draw_layer) {
    SIGNAL_DRAW_LAYER_CHANGED.emit(this, old_draw_layer + draw_layer);
}

void Node2D::addedToNode(Node* parent_node) {
    Node::addedToNode(parent_node);
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(parent_node)) {
        return parent_2d->SIGNAL_DRAW_LAYER_CHANGED.connect(&Node2D::onParentDrawLayerChanged, this, "Child" + int2str(getId()));
    }
}
void Node2D::removedFromNode(Node* former_parent_node) {
    Node::removedFromNode(former_parent_node);
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(former_parent_node)) {
        return parent_2d->SIGNAL_DRAW_LAYER_CHANGED.disconnect("Child" + int2str(getId()));
    }
}
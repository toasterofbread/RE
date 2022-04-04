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
        
        text += " | " + (string)int2char(getId());

        Vector2 global_position = getGlobalPosition();
        text += " | " + vector2str(global_position, 0);
        markPosition(global_position, text, BLACK);
    }
}

Colour Node2D::getModulate() {
    return modulate;
}

Colour Node2D::getGlobalModulate() {
    if (!modulate_relative_to_parent) {
        return modulate;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalModulate() * modulate;
    }

    return modulate;
}

Vector2 Node2D::getPosition() {
    return position;
}
void Node2D::setPosition(Vector2 value) {
    if (position == value) {
        return;
    }

    if (SIGNAL_GLOBAL_POSITION_CHANGED.getConnectionCount() == 0) {
        position = value;
        return;
    }

    Vector2 original_global_position = position;
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        original_global_position += parent_2d->getGlobalPosition();
    }

    position = value;

    SIGNAL_GLOBAL_POSITION_CHANGED.emit(original_global_position);
}

Vector2 Node2D::getGlobalPosition() {
    
    if (!position_relative_to_parent) {
        return getPosition();
    }

    assert(isInsideTree());
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalPosition() + getPosition();
    }
    
    return getPosition();
}
void Node2D::setGlobalPosition(Vector2 value) {

    if (!position_relative_to_parent) {
        setPosition(value);
        return;
    }

    assert(isInsideTree());

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        setPosition(value - parent_2d->getGlobalPosition());
    }
    else {
        setPosition(value);
    }
}

Vector2 Node2D::getScale() {
    return scale;
}
void Node2D::setScale(Vector2 value) {
    if (scale == value) {
        return;
    }

    if (!SIGNAL_GLOBAL_SCALE_CHANGED.getConnectionCount() == 0) {
        scale = value;
        return;
    }

    Vector2 original_global_scale = scale;
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        original_global_scale *= parent_2d->getGlobalScale();
    }

    scale = value;

    SIGNAL_GLOBAL_SCALE_CHANGED.emit(original_global_scale);
}

Vector2 Node2D::getGlobalScale() {

    if (!scale_relative_to_parent) {
        return getScale();
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalScale() * getScale();
    }

    return getScale();
}
void Node2D::setGlobalScale(Vector2 value) {

    if (!scale_relative_to_parent) {
        setScale(value);
        return;
    }

    assert(isInsideTree());
    setScale(value / getGlobalScale());
}

float Node2D::getRotation() {
    return rotation;
}
void Node2D::setRotation(float value) {

    if (rotation == value) {
        return;
    }

    if (!SIGNAL_GLOBAL_ROTATION_CHANGED.getConnectionCount() == 0) {
        rotation = value;
        return;
    }

    float original_global_rotation = rotation;
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        original_global_rotation += parent_2d->getGlobalRotation();
    }

    rotation = value;

    SIGNAL_GLOBAL_ROTATION_CHANGED.emit(original_global_rotation);
}

float Node2D::getGlobalRotation() {

    if (!rotation_relative_to_parent) {
        return getRotation();
    }

    assert(isInsideTree());
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->getGlobalRotation() + getRotation();
    }

    return getRotation();
}
void Node2D::setGlobalRotation(float value) {
    assert(isInsideTree());

    if (!rotation_relative_to_parent) {
        setRotation(value);
        return;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        setRotation(value - parent_2d->getGlobalRotation());
        return;
    }

    setRotation(value);
}

void Node2D::setVisible(bool value) {
    visible = value;
}

void Node2D::setGlobalVisible(bool value) {
    if (!visible_relative_to_parent) {
        visible = value;
        return;
    }

    if (!value) {
        visible = false;
        return;
    }
    
    visible = true;

    if (!getParent()->isRoot()) {
        if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
            parent_2d->setGlobalVisible(true);
        }
    }
}

bool Node2D::isVisible() {
    return visible;
}

bool Node2D::isGlobalVisible() {
    
    if (!visible) {
        return false;
    }
    
    if (!visible_relative_to_parent) {
        return visible;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return parent_2d->isGlobalVisible();
    }

    return visible;
}

void Node2D::setDrawLayer(int value) {
    if (value == draw_layer) {
        return;
    }

    int parent_draw_layer = 0;
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        parent_draw_layer = parent_2d->getGlobalDrawLayer();
    }

    int original_draw_layer = parent_draw_layer + draw_layer;
    draw_layer = value; 
    SIGNAL_DRAW_LAYER_CHANGED.emit(original_draw_layer, parent_draw_layer + draw_layer); 
}

void Node2D::setGlobalDrawLayer(int value) {
    if (!draw_layer_relative_to_parent) {
        setDrawLayer(value);
        return;
    }

    int parent_draw_layer = 0;
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        parent_draw_layer = parent_2d->getGlobalDrawLayer();

        int original_draw_layer = parent_draw_layer + draw_layer;
        draw_layer = clamp(value - parent_draw_layer, SceneTree::MIN_DRAW_LAYER, SceneTree::MAX_DRAW_LAYER);

        if (parent_draw_layer + draw_layer == original_draw_layer) {
            return;
        }

        SIGNAL_DRAW_LAYER_CHANGED.emit(original_draw_layer, parent_draw_layer + draw_layer);
        return;
    }

    int original_draw_layer = draw_layer;
    draw_layer = value; 
    SIGNAL_DRAW_LAYER_CHANGED.emit(original_draw_layer, draw_layer); 
}

int Node2D::getDrawLayer() {
    return draw_layer;
}

int Node2D::getGlobalDrawLayer() {
    
    if (!draw_layer_relative_to_parent) {
        return draw_layer;
    }

    if (Node2D* parent_2d = dynamic_cast<Node2D*>(getParent())) {
        return clamp(parent_2d->getGlobalDrawLayer() + draw_layer, SceneTree::MIN_DRAW_LAYER, SceneTree::MAX_DRAW_LAYER);
    }

    return draw_layer;
}

void Node2D::onParentDrawLayerChanged(int old_draw_layer, int new_draw_layer) {
    SIGNAL_DRAW_LAYER_CHANGED.emit(old_draw_layer + draw_layer, new_draw_layer + draw_layer);
}

void Node2D::addedToNode(Node* parent_node) {
    Node::addedToNode(parent_node);
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(parent_node)) {
        parent_2d->SIGNAL_DRAW_LAYER_CHANGED.connect(&Node2D::onParentDrawLayerChanged, this);

        parent_2d->SIGNAL_GLOBAL_POSITION_CHANGED.connect(&Node2D::onParentGlobalPositionChanged, this);
        parent_2d->SIGNAL_GLOBAL_ROTATION_CHANGED.connect(&Node2D::onParentGlobalRotationChanged, this);
        parent_2d->SIGNAL_GLOBAL_SCALE_CHANGED.connect(&Node2D::onParentGlobalScaleChanged, this);

    }
}
void Node2D::removedFromNode(Node* former_parent_node) {
    Node::removedFromNode(former_parent_node);
    
    if (Node2D* parent_2d = dynamic_cast<Node2D*>(former_parent_node)) {
        parent_2d->SIGNAL_DRAW_LAYER_CHANGED.disconnect(this);

        parent_2d->SIGNAL_GLOBAL_POSITION_CHANGED.disconnect(this);
        parent_2d->SIGNAL_GLOBAL_ROTATION_CHANGED.disconnect(this);
        parent_2d->SIGNAL_GLOBAL_SCALE_CHANGED.disconnect(this);
    }
}

void Node2D::onParentGlobalPositionChanged(Vector2 old_global_position) {
    SIGNAL_GLOBAL_POSITION_CHANGED.emit(old_global_position + getPosition());
}

void Node2D::onParentGlobalRotationChanged(float old_global_rotation) {
    SIGNAL_GLOBAL_ROTATION_CHANGED.emit(old_global_rotation + getRotation());
}


void Node2D::onParentGlobalScaleChanged(Vector2 old_global_scale) {
    SIGNAL_GLOBAL_SCALE_CHANGED.emit(old_global_scale + getScale());
}
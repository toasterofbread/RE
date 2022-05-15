#include "node_3d.h"

#include "common/colour.h"
#include "common/draw.h"
#include "node/types/camera_3d.h"

Node3D* Node3D::getFirst3DParent(bool only_immediate_parent) {
    Node* ret = this;
    do {
        ret = ret->getParent();
        if (Node3D* parent_3d = dynamic_cast<Node3D*>(ret)) {
            return parent_3d;
        }
    } while (ret != NULL && !only_immediate_parent && !ret->isRoot());
    return NULL;
}

void Node3D::ready() {
    super::ready();
}

void Node3D::draw() {

    if (show_gizmos && inFrontOfCamera(getGlobalPosition())) {
        string text = getTypeName();
        if (getName() != text) {
            text += " | " + getName();
        }
        if (!isVisible()) {
            text += " (invisible)";
        }
        
        text += " | " + (string)to_string(getId());

        Vector3 position = getGlobalPosition();
        Vector2 draw_position = GetWorldToScreen(position, *Engine::getSingleton()->getTree()->getEnabledCamera3D()->getCamera());

        text += " | " + position.toString();

        for (string suffix : additional_gizmos) {
            text += " | ";
            text += suffix;
        }

        markPosition(100, draw_position, text, Colour::GREEN());

        draw_position.y -= 15;
        for (string line : additional_gizmos_unique) {
            draw_position.y -= 15;

            Draw::drawOnLayer(100, true, Draw::drawText, line, draw_position, Colour::BLACK(), 1.0f, false);
        }

        if (getId() == 3) {
            Draw::drawCube(getGlobalPosition() + Vector3(0, 2, 0), Vector3(1, 1, 1), Colour::BLUE());
        }
    }

    additional_gizmos.clear();
    additional_gizmos_unique.clear();
}

Colour Node3D::getModulate() {
    return modulate;
}

Colour Node3D::getGlobalModulate() {
    if (!modulate_relative_to_parent) {
        return modulate;
    }

    if (Node3D* parent_3d = getFirst3DParent()) {
        return parent_3d->getGlobalModulate() * modulate;
    }

    return modulate;
}

Vector3 Node3D::getPosition() {
    return Vector3(position);
}
void Node3D::setPosition(Vector3 value) {
    if (position == value) {
        return;
    }

    if (SIGNAL_GLOBAL_POSITION_CHANGED.getConnectionCount() == 0) {
        position = value;
        return;
    }

    if (isInsideTree()) {
        Vector3 original_global_position = position;
        if (Node3D* parent_3d = getFirst3DParent()) {
            original_global_position += parent_3d->getGlobalPosition();
        }
        position = value;
        SIGNAL_GLOBAL_POSITION_CHANGED.emit(original_global_position);
    }
    else {
        position = value;
    }
}

Vector3 Node3D::getGlobalPosition() {
    
    if (!position_relative_to_parent) {
        return getPosition();
    }

    ASSERT_MSG(isInsideTree(), "Node must be inside tree to acquire global position");
    
    if (Node3D* parent_3d = getFirst3DParent()) {
        return parent_3d->getGlobalPosition() + getPosition();
    }
    
    return getPosition();
}

void Node3D::setGlobalPosition(Vector3 value) {

    if (!position_relative_to_parent) {
        setPosition(value);
        return;
    }

    ASSERT(isInsideTree());

    if (Node3D* parent_3d = getFirst3DParent()) {
        setPosition(value - parent_3d->getGlobalPosition());
    }
    else {
        setPosition(value);
    }
}

Vector3 Node3D::getScale() {
    return Vector3(scale);
}
void Node3D::setScale(Vector3 value) {
    if (scale == value) {
        return;
    }

    if (SIGNAL_GLOBAL_SCALE_CHANGED.getConnectionCount() == 0) {
        scale = value;
        return;
    }

    Vector3 original_global_scale = scale;
    if (Node3D* parent_3d = getFirst3DParent()) {
        original_global_scale *= parent_3d->getGlobalScale();
    }

    scale = value;

    SIGNAL_GLOBAL_SCALE_CHANGED.emit(original_global_scale);
}

Vector3 Node3D::getGlobalScale() {

    if (!scale_relative_to_parent) {
        return getScale();
    }

    if (Node3D* parent_3d = getFirst3DParent()) {
        return parent_3d->getGlobalScale() * getScale();
    }

    return getScale();
}
void Node3D::setGlobalScale(Vector3 value) {

    if (!scale_relative_to_parent) {
        setScale(value);
        return;
    }

    ASSERT(isInsideTree());
    setScale(value / getGlobalScale());
}

Quaternion Node3D::getRotation() {
    return rotation;
}

void Node3D::setRotation(Quaternion value) {
    if (rotation == value) {
        return;
    }

    ASSERT(value.isValid());

    if (SIGNAL_GLOBAL_ROTATION_CHANGED.getConnectionCount() == 0) {
        rotation = value;
        return;
    }

    Quaternion original_global_rotation = getRotation();
    if (Node3D* parent_3d = getFirst3DParent()) {
        original_global_rotation = QuaternionAdd(original_global_rotation, parent_3d->getGlobalRotation());
    }

    rotation = value;

    SIGNAL_GLOBAL_ROTATION_CHANGED.emit(original_global_rotation);

}

Quaternion Node3D::getGlobalRotation() {

    if (!rotation_relative_to_parent) {
        return getRotation();
    }

    ASSERT(isInsideTree());
    
    if (Node3D* parent_3d = getFirst3DParent()) {
        return QuaternionMultiply(parent_3d->getGlobalRotation(), getRotation());
    }

    return getRotation();
}
void Node3D::setGlobalRotation(Quaternion value) {
    ASSERT(isInsideTree());

    if (!rotation_relative_to_parent) {
        setRotation(value);
        return;
    }

    if (Node3D* parent_3d = getFirst3DParent()) {
        Vector3 parent_rotation = QuaternionToEuler(parent_3d->getGlobalRotation());
        Vector3 target = (InternalVector3)QuaternionToEuler(value) - parent_rotation;
        Vector3 value_euler = QuaternionToEuler(value);
        setRotation(QuaternionFromEuler(target.x, target.y, target.z));
        
        

        // setRotation(QuaternionMultiply(value, QuaternionInvert(parent_3d->getGlobalRotation())));
        
        return;
    }

    setRotation(value);
}

void Node3D::setVisible(bool value) {
    visible = value;
}

void Node3D::setGlobalVisible(bool value) {
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
        if (Node3D* parent_3d = getFirst3DParent()) {
            parent_3d->setGlobalVisible(true);
        }
    }
}

bool Node3D::isVisible() {
    return visible;
}

bool Node3D::isGlobalVisible() {
    
    if (!visible) {
        return false;
    }
    
    if (!visible_relative_to_parent) {
        return visible;
    }

    if (Node3D* parent_3d = getFirst3DParent()) {
        return parent_3d->isGlobalVisible();
    }

    return visible;
}

void Node3D::setDrawLayer(int value) {
    if (value == draw_layer) {
        return;
    }

    int parent_draw_layer = 0;
    if (Node3D* parent_3d = getFirst3DParent()) {
        parent_draw_layer = parent_3d->getGlobalDrawLayer();
    }

    int original_draw_layer = parent_draw_layer + draw_layer;
    draw_layer = value; 
    SIGNAL_DRAW_LAYER_CHANGED.emit(original_draw_layer, parent_draw_layer + draw_layer); 
}

void Node3D::setGlobalDrawLayer(int value) {
    if (!draw_layer_relative_to_parent) {
        setDrawLayer(value);
        return;
    }

    int parent_draw_layer = 0;
    if (Node3D* parent_3d = getFirst3DParent()) {
        parent_draw_layer = parent_3d->getGlobalDrawLayer();

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

int Node3D::getDrawLayer() {
    return draw_layer;
}

int Node3D::getGlobalDrawLayer() {
    
    if (!draw_layer_relative_to_parent) {
        return draw_layer;
    }

    if (Node3D* parent_3d = getFirst3DParent()) {
        return clamp(parent_3d->getGlobalDrawLayer() + draw_layer, SceneTree::MIN_DRAW_LAYER, SceneTree::MAX_DRAW_LAYER);
    }

    return draw_layer;
}

void Node3D::onParentDrawLayerChanged(int old_draw_layer, int new_draw_layer) {
    SIGNAL_DRAW_LAYER_CHANGED.emit(old_draw_layer + draw_layer, new_draw_layer + draw_layer);
}

void Node3D::addedToNode() {
    super::addedToNode();
    if (Node3D* parent_3d = getFirst3DParent()) {
        parent_3d->SIGNAL_DRAW_LAYER_CHANGED.connect(this, &Node3D::onParentDrawLayerChanged);

        parent_3d->SIGNAL_GLOBAL_POSITION_CHANGED.connect(this, &Node3D::onParentGlobalPositionChanged);
        parent_3d->SIGNAL_GLOBAL_ROTATION_CHANGED.connect(this, &Node3D::onParentGlobalRotationChanged);
        parent_3d->SIGNAL_GLOBAL_SCALE_CHANGED.connect(this, &Node3D::onParentGlobalScaleChanged);
    }
}
void Node3D::removedFromNode(Node* former_parent_node) {
    super::removedFromNode(former_parent_node);
    
    if (Node3D* parent_3d = getFirst3DParent()) {
        parent_3d->SIGNAL_DRAW_LAYER_CHANGED.disconnect(this);

        parent_3d->SIGNAL_GLOBAL_POSITION_CHANGED.disconnect(this);
        parent_3d->SIGNAL_GLOBAL_ROTATION_CHANGED.disconnect(this);
        parent_3d->SIGNAL_GLOBAL_SCALE_CHANGED.disconnect(this);
    }
}

void Node3D::onParentGlobalPositionChanged(Vector3 old_global_position) {
    SIGNAL_GLOBAL_POSITION_CHANGED.emit(old_global_position + getPosition());
}

void Node3D::onParentGlobalRotationChanged(Quaternion old_global_rotation) {
    SIGNAL_GLOBAL_ROTATION_CHANGED.emit(QuaternionAdd(old_global_rotation, getRotation()));
}

void Node3D::onParentGlobalScaleChanged(Vector3 old_global_scale) {
    SIGNAL_GLOBAL_SCALE_CHANGED.emit(old_global_scale + getScale());
}

bool Node3D::inFrontOfCamera(Vector3 global_position) {

    if (!isInsideTree()) {
        return false;
    }

    if (Camera3D* camera = getTree()->getEnabledCamera3D()) {
        RayCam3D* cam = camera->getCamera();
        return (global_position - cam->position).dot(Vector3Subtract(cam->target, cam->position)) > 0;
    }

    return false;
}
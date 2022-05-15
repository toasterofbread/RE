#include "camera_2d.h"

void Camera2D::init() {
    super::init();
    camera.offset = Vector2::ZERO();
    camera.target = Vector2::ZERO();
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void Camera2D::kill() {
    super::kill();
    disable();
}

bool Camera2D::isEnabled() {
    if (isInsideTree()) {
        return getTree()->getEnabledCamera2D() == this;
    }
    return false;
}

void Camera2D::enable() {
    ASSERT(isInsideTree());
    getTree()->setEnabledCamera2D(this);
}

void Camera2D::disable() {
    if (!isEnabled()) {
        return;
    }
    getTree()->setEnabledCamera2D(NULL);
}

void Camera2D::setZoom(float value) {
    camera.zoom = value;
}

float Camera2D::getZoom() {
    return camera.zoom;
}

Vector2 Camera2D::getCenter() {
    return getGlobalPosition();
}

void Camera2D::removedFromNode(Node* former_parent_node) {
    disable();
    super::removedFromNode(former_parent_node);
}

void Camera2D::onParentGlobalPositionChanged(Vector2 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);
    camera.target = getGlobalPosition() - (OS::getScreenSize() / 2.0f);
}

void Camera2D::onParentGlobalRotationChanged(float old_global_rotation) {
    super::onParentGlobalRotationChanged(old_global_rotation);
    camera.rotation = RAD2DEG(getGlobalRotation());
}
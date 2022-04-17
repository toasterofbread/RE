#include "camera.h"

void Camera::kill() {
    super::kill();
    disable();
}

bool Camera::isEnabled() {
    if (isInsideTree()) {
        return getTree()->getEnabledCamera() == this;
    }
    return false;
}

void Camera::enable() {
    assert(isInsideTree());
    getTree()->setEnabledCamera(this);
}

void Camera::disable() {
    if (!isEnabled()) {
        return;
    }
    getTree()->setEnabledCamera(NULL);
}

Vector2 Camera::getGlobalOffset() {
    Vector2 ret = getGlobalPosition();
    ret.x -= OS::getScreenWidth() / 2.0f;
    ret.y -= OS::getScreenHeight() / 2.0f;
    return ret * -1.0f;
}

void Camera::removedFromNode(Node* former_parent_node) {
    disable();
    super::removedFromNode(former_parent_node);
}
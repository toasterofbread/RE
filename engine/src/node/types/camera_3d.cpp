#include "camera_3d.h"

#include "raylib/raymath.h"

#define MAX_FOV 100.0f
#define MIN_FOV 10.0f

void Camera3D::ready() {
    super::ready();
    if (getEnableOnReady()) {
        enable();
    }
}

void Camera3D::kill() {
    super::kill();
    disable();
}

bool Camera3D::isEnabled() {
    if (isInsideTree()) {
        return getTree()->getEnabledCamera3D() == this;
    }
    return false;
}

void Camera3D::enable() {
    ASSERT(isInsideTree());
    getTree()->setEnabledCamera3D(this);
}

void Camera3D::disable() {
    if (!isEnabled()) {
        return;
    }
    getTree()->setEnabledCamera3D(NULL);
}

void Camera3D::setTarget(Vector3 value) {
    camera.target = value;
}

Vector3 Camera3D::getTarget() {
    return camera.target;
}

Vector3 Camera3D::getCenter() {
    return getGlobalPosition();
}

void Camera3D::removedFromNode(Node* former_parent_node) {
    disable();
    super::removedFromNode(former_parent_node);
}

void Camera3D::onParentGlobalPositionChanged(Vector3 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);
    updateCamera();
}

void Camera3D::onParentGlobalRotationChanged(Vector3 old_global_rotation) {
    super::onParentGlobalRotationChanged(old_global_rotation);
    updateCamera();
}

void Camera3D::setZoom(float value) {
    ASSERT(value <= 1.0f && value >= 0.0f);
    camera.fovy = MIN_FOV + ((MAX_FOV - MIN_FOV) * value);
}

float Camera3D::getZoom() {
    return (camera.fovy - MIN_FOV) / (MAX_FOV - MIN_FOV);
}

void Camera3D::setPosition(Vector3 value) {
    super::setPosition(value);
    updateCamera();
}

void Camera3D::setRotation(Vector3 value) {
    super::setRotation(value);
    updateCamera();
}

void Camera3D::updateCamera() {
    camera.position = getGlobalPosition();

    Vector3 rotation = getGlobalRotation();
    Matrix transform = MatrixMultiply(MatrixTranslate(0, 0, 1), MatrixRotateXYZ(Vector3(PI * 2 - rotation.y, PI * 2 - rotation.x, 0.0f)));
    camera.target.x = camera.position.x - transform.m12;
    camera.target.y = camera.position.y - transform.m13;
    camera.target.z = camera.position.z - transform.m14;
}
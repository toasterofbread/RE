#include "camera_3d.h"

#include "raylib/raymath.h"
#include "common/draw.h"

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
    updateFrustum();
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

bool Camera3D::isPointVisibleInFrustum(Vector3 point) {
    for (int face = 0; face < Camera3D::Frustum::FACE_COUNT - 1; face++) {
        if (frustum.faces[face].getDistanceTo(Vector3Transform(point, matrix)) < 0.0f) {
            return false;
        }
    }
    return true;
}

void Camera3D::updateCamera() {
    camera.position = getGlobalPosition();

    Vector3 rotation = getGlobalRotation();
    Matrix transform = MatrixMultiply(MatrixTranslate(0, 0, 1), MatrixRotateXYZ(Vector3(PI * 2 - rotation.y, PI * 2 - rotation.x, 0.0f)));
    camera.target.x = camera.position.x - transform.m12;
    camera.target.y = camera.position.y - transform.m13;
    camera.target.z = camera.position.z - transform.m14;

    updateFrustum();
}

void Camera3D::updateFrustum() {
    float hh = tan(DEG2RAD(camera.fovy) * 0.5) * CAM_NEAR;
    float hw = hh * OS::getAspectRatio();

    Vector3 nw = Vector3(-hw, hh, 1.0);
    Vector3 ne = Vector3(hw, hh, 1.0);
    Vector3 se = Vector3(hw, -hh, 1.0);
    Vector3 sw = Vector3(-hw, -hh, 1.0);

    frustum.faces[Frustum::TOP].set(nw.cross(ne));
    frustum.faces[Frustum::RIGHT].set(ne.cross(se));
    frustum.faces[Frustum::BOTTOM].set(se.cross(sw));
    frustum.faces[Frustum::LEFT].set(sw.cross(nw));
    frustum.faces[Frustum::NEAR].set(Vector3::BACK());
    frustum.faces[Frustum::FAR].set(Vector3::FRONT());

    matrix = GetCameraMatrix(camera);
}

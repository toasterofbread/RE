#include "world.h"

#include "common/draw.h"
#include "common/aabb.h"
#include "node/types/camera_3d.h"

void World::Chunk::setup(Vector2 grid_pos) {
    grid_position = grid_pos;
    setPosition(Vector3((grid_pos * CHUNK_SIZE).x, 0.0f, (grid_pos * CHUNK_SIZE).y));
}

void World::Chunk::draw() {
    super::draw();

    Vector3 position = getGlobalPosition();

    if (!isVisible(getTree()->getEnabledCamera3D(), position)) {
        return;
    }

    Draw::drawCube(position, Vector3(1, 1, 1), Colour(0.5f, 0.0f, 0.0f, 0.5f));
}

float get2dRotation(Vector3 rotation) {
    float angle;
    Vector3 axis;
    Quaternion rot = QuaternionFromEuler(rotation.z, rotation.y, rotation.x);
    QuaternionToAxisAngle({ 0.0f, rot.y, 0.0f, rot.w }, &axis, &angle);
    return angle;
}

Vector3 Player_GetForwardVector(Vector3 rotation) {
    float cx = cosf(rotation.x);
    float sx = sinf(rotation.x);
    
    float sy = sinf(rotation.y);
    float cy = cosf(rotation.y);
    
    return (Vector3) {cx * sy, cy, sx * sy};
}

bool World::Chunk::isVisible(Camera3D* camera, Vector3 global_position) {

    Camera3D::Frustum& frustum = camera->getFrustum();
    AABB bounding_box = { Vector3Transform(global_position, GetCameraMatrix(*camera->getCamera())), Vector3(0.5f, 0.5f, 0.5f) };

    for (int face = 0; face < Camera3D::Frustum::FACE_COUNT - 1; face++) {

        const float r = bounding_box.extents.x * std::abs(frustum.faces[face].normal.x) +
                bounding_box.extents.y * std::abs(frustum.faces[face].normal.y) + bounding_box.extents.z * std::abs(frustum.faces[face].normal.z);

        if (frustum.faces[face].getDistanceTo(bounding_box.center) < -r) {
            return false;
        }
    }
    
    return true;
}

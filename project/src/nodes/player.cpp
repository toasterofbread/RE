#include "player.h"

#include "common/draw.h"
#include "common/enums.h"
#include "common/input.h"
#include "node/types/camera_3d.h"

#define MOUSE_SENSITIVITY 0.25f
#define MOVEMENT_SPEED_H 9.0f
#define MOVEMENT_SPEED_V 5.0f
#define CAMERA_Y_CLAMP_MARGIN 0.0001f

Camera3D* camera;
CollisionShape3D* shape;

void Player::ready() {
    super::ready();
 
    // setKinematic(true);
    // setApplyGravity(false);

    setPosition(Vector3(-10, 10, -10));
    // setLinearVelocity(Vector3(0, 0, 0));

    // setFixedRotation(true);

    shape = new CollisionShape3D;
    shape->setBoxShape(Vector3(0.5f, 1.8f, 0.5f));
    addChild(shape);

    // dGeomSetCollideBits(shape->getShape(), 0x0002);
    // dGeomSetCategoryBits(shape->getShape(), 0x0001);

    // dMatrix3 R;
    // dRFromAxisAndAngle(R, 1.0f, 0, 0, DEG2RAD(90.0f));
    // dBodySetRotation(getBody(), R);

    camera = new Camera3D;
    addChild(camera);
    camera->enable();
}

Vector2 angle = Vector2(-PI/2, 0.0f);
void Player::process(float delta) {
    super::process(delta);

    // Get the rotation delta from mouse movement or left joystick
    Vector2 rotation_delta = Vector2(GetMouseDelta());
    if (rotation_delta.isZero()) {
        rotation_delta = Input::getAnalogPadVector(SIDE::RIGHT).normalised() * 1800.0f;
    }

    // Apply rotation delta to the final rotation
    angle.x -= rotation_delta.x * delta * MOUSE_SENSITIVITY;
    angle.y = clamp(angle.y - rotation_delta.y * delta * MOUSE_SENSITIVITY, -PI / 2 + CAMERA_Y_CLAMP_MARGIN, PI / 2 - CAMERA_Y_CLAMP_MARGIN);
    setRotation(QuaternionFromEuler(angle.y, angle.x, 0));

    Vector3 position = getPosition();

    // Apply DPad or joystick value to movement
    Vector2 h_movement = Input::getPadVector();
    if (h_movement.isZero()) {
        h_movement = Input::getAnalogPadVector(SIDE::LEFT);
    }
    h_movement.normalise();

    Vector3 velocity;
    if (!h_movement.isZero())
        velocity = Vector3(h_movement.x, 0.0f, h_movement.y).rotatedByQuat(getGlobalRotation()) * MOVEMENT_SPEED_H;

    #define APPREQ(a, b) (abs(a - b) < 0.00001)
    // for (CollisionShape3D::Contact* contact : shape->contacts) {
    //     Vector3 normal = contact->getNormal();
    //     for (int i = 0; i < 3; i++) {
    //         if (!APPREQ(normal[i], 0.0) && sign(normal[i]) != sign(velocity[i])) {
    //             velocity[i] = 0;
    //         }
    //     }
    // }

    // OS::dbPrint("Player contacts: " + to_string(shape->contacts.size()));
    OS::dbPrint("Player velocity: " + velocity.toString());
    OS::dbPrint("Player rotation: " + getRotation().toString());
    OS::dbPrint("Player position: " + getPosition().toString());
   
    // Vertical movement
    int v_movement = INPUT_EVENT_JUMP.isTriggered() - INPUT_EVENT_CROUCH.isTriggered();
    velocity.y = v_movement * MOVEMENT_SPEED_V;

    // Apply final velocity
    setLinearVelocity(velocity);
    // setPosition(getPosition() + velocity * delta);

    // Quaternion rot = QuaternionFromMatrix(MatrixAdd(MatrixLookAt(Vector3::ZERO(), velocity, Vector3::UP()), MatrixRotateY(DEG2RAD(180))));
    // OS::dbPrint("Rot: " + Vector3(QuaternionToEuler(rot)).toString());
    // if (!velocity.isZero()) {
    //     float quat[4];
    //     quat[1] = rot.x;
    //     quat[2] = rot.y;
    //     quat[3] = rot.z;
    //     quat[0] = rot.w;
    //     dBodySetQuaternion(getBody(), quat);
    // }

    // const float* body_pos = dGeomGetPosition(((CollisionShape3D*)getChild("CollisionShape3D"))->getShape());
    // OS::dbPrint("Player pos: " + Vector3(body_pos[0], body_pos[1], body_pos[2]).toString());

    // Zoom camera with mouse wheel (fov)
    camera->setZoom(min(max(camera->getZoom() + (GetMouseWheelMove() * -0.1f), 0.0f), 1.0f));

    // !todo move to world process
    Chunk* chunk = world->getChunk(position.x, position.z, false);
    if (chunk)
        world->starting_chunk = chunk->sub_chunks[SUBCHUNK_COUNT - 1];
    else
        world->starting_chunk = NULL;

    // Calculate camera view ray
    Ray view_ray;
    view_ray.position = position;
    view_ray.direction = Vector3Normalize(Vector3Subtract(camera->getCamera()->target, camera->getCamera()->position));

    // Get list of subchunks intersecting view ray, in order of proximity to player
    list<pair<SubChunk*, float>> intersecting;
    chunk = world->chunks;
    while (chunk) {
        
        // Check if view ray intersects chunk bounding box
        if (GetRayCollisionBox(view_ray, chunk->bounding_box).hit) {

            // Iterate through subchunks
            for (int i = 0; i < SUBCHUNK_COUNT; i++) {
                SubChunk* sub = chunk->sub_chunks[i];

                // Check if view ray intersects subchunk bounding box
                RayCollision collision = GetRayCollisionBox(view_ray, sub->bounding_box);
                if (collision.hit) {

                    // First colliding subchunk can go directly into list
                    if (intersecting.empty()) {
                        intersecting.push_back(make_pair(sub, collision.distance));
                    }
                    else {
                        // Find the first intersected subchunk further away than this one
                        bool found = false;
                        for (auto i = intersecting.begin(); i != intersecting.end(); i++) {
                            if (collision.distance < i->second) {
                                intersecting.insert(i, make_pair(sub, collision.distance));
                                found = true;
                                break;
                            }
                        }

                        // If no further subchunk was found, this one is the furthest away
                        if (!found)
                            intersecting.push_back(make_pair(sub, collision.distance));
                    }
                }
            }
        }

        chunk = chunk->next;
    }

    looking_at_block = NULL;
    looking_at_face = DIRECTION_3::NONE;

    // Iterate through intersected subchunks
    BoundingBox box;
    for (auto i : intersecting) {

        SubChunk* chunk = i.first;
        Vector3 chunk_position = chunk->chunk->getGlobalPosition();
        chunk_position.y = 0;

        float closest_distance = -1.0f;
        Vector3 closest_point;

        // Iterate through subchunk blocks
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = SUBCHUNK_HEIGHT * chunk->index; y < SUBCHUNK_HEIGHT * (chunk->index + 1); y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    Block* block = chunk->chunk->getBlock(x, y, z);
                    if (block == NULL) {
                        continue;
                    }

                    // Check if view ray intersects with block
                    block->getBoundingBox(&box, chunk_position);
                    RayCollision collision = GetRayCollisionBox(view_ray, box);

                    // Overwrite looking_at_block if block is closer
                    if (collision.hit && (closest_distance == -1.0f || collision.distance < closest_distance)) {
                        looking_at_block = block;
                        closest_distance = collision.distance;
                        closest_point = collision.point;
                    }
                }
            }
        }

        // End iteration if block was found
        if (looking_at_block) {
            looking_at_face = looking_at_block->getNearestFace(closest_point);
            Draw::markPoint(closest_point, Colour::BLUE());
            break;
        }
    }

    if (looking_at_block != NULL) {

        if (INPUT_EVENT_ATTACK.isJustTriggered()) {
            looking_at_block->type = Block::TYPE::AIR;
            looking_at_block->subchunk->requestMeshGeneration();
        }

        if (INPUT_EVENT_INTERACT.isJustTriggered()) {
            OS::print("Block position: " + Vector3(looking_at_block->x, looking_at_block->y, looking_at_block->z).toString());
            OS::print("Face: " + directionToString(looking_at_face));

            // const float* pos = dGeomGetPosition(looking_at_block->subchunk->getShape());
            // const float* off = dGeomGetOffsetPosition(looking_at_block->subchunk->getShape());

            // OS::print("Geom pos: " + Vector3(pos[0] + off[0], pos[1] + off[1], pos[2] + off[2]).toString());
            OS::print("Subchunk pos: " + looking_at_block->subchunk->getGlobalPosition().toString());
            OS::print("Subchunk id: " + to_string(looking_at_block->subchunk->getId()));
            OS::print("-------------------");
        }
    }

}

void Player::draw() {
    if (looking_at_block) {
        #define SIZE 1.01f
        // Draw::drawBoundingBox(looking_at_block->getBoundingBox(), Colour(0.9, 0.9, 0.9), Vector3::ZERO(), Vector3(SIZE, SIZE, SIZE));
    }

    #define CROSSHAIR_SIZE 10
    Vector2 center = OS::getScreenSize() / 2.0;
    Draw::drawLine(center - Vector2(CROSSHAIR_SIZE, 0), center + Vector2(CROSSHAIR_SIZE, 0), Colour::BLACK());
    Draw::drawLine(center - Vector2(0, CROSSHAIR_SIZE), center + Vector2(0, CROSSHAIR_SIZE), Colour::BLACK());
}

#include "player.h"

#include "common/draw.h"
#include "common/enums.h"
#include "common/input.h"
#include "node/types/camera_3d.h"

#define MOUSE_SENSITIVITY 1.5f

Camera3D* camera;

void Player::ready() {
    super::ready();
 
    // setKinematic(true);
    setApplyGravity(false);

    setPosition(Vector3(-62, 10, 15));
    setLinearVelocity(Vector3(0, 0, 0));

    CollisionShape3D* shape = new CollisionShape3D;
    shape->setBoxShape(Vector3(1, 2, 1));
    addChild(shape);
    dGeomSetCollideBits(shape->getShape(), 0x0002);
    dGeomSetCategoryBits(shape->getShape(), 0x0001);

    camera = new Camera3D;
    addChild(camera);
    camera->enable();
}

void Player::process(float delta) {
    super::process(delta);

    // Apply mouse or analog stick movement to rotation
    Vector2 rotation_delta = -Vector2(GetMouseDelta());
    if (rotation_delta.isZero()) {
        rotation_delta = Input::getAnalogPadVector(SIDE::RIGHT) * -1800.0f * delta;
    }
    Vector3 rotation = getRotation();
    rotation = (rotation + (Vector3(rotation_delta.x, rotation_delta.y, 0.0f) * MOUSE_SENSITIVITY * 0.001f * (camera->getCamera()->fovy / 45.0f))).clampAngle();
    setRotation(rotation);

    OS::dbPrint("Player rotation: " + rotation.toString());
   
    Vector3 position = getPosition();
    Vector3 velocity = Vector3::ZERO();

    // Apply DPad or joystick value to movement
    Vector2 h_movement = Input::getPadVector();
    if (h_movement.isZero()) {
        h_movement = Input::getAnalogPadVector(SIDE::LEFT);
    }
    velocity = velocity.move(h_movement, getGlobalRotation(), 500.0f * delta);

    // Vertical movement
    int v_movement = INPUT_EVENT_JUMP.isTriggered() - INPUT_EVENT_CROUCH.isTriggered();
    velocity.y += v_movement * delta * 400.0f;

    // Apply final velocity
    setLinearVelocity(velocity);

    const float* body_pos = dGeomGetPosition(((CollisionShape3D*)getChild("CollisionShape3D"))->getShape());
    OS::dbPrint("Player pos: " + Vector3(body_pos[0], body_pos[1], body_pos[2]).toString());

    // Zoom camera with mouse wheel (fov)
    // camera->setZoom(min(max(camera->getZoom() + (GetMouseWheelMove() * -0.1f), 0.0f), 1.0f));

    // !todo move to world process
    Chunk* chunk = world->getChunk(position.x, position.z, false);
    if (chunk)
        world->starting_chunk = chunk->sub_chunks[SUBCHUNK_COUNT - 1];
    else
        world->starting_chunk = NULL;

    // Calculate camera view ray
    Ray view_ray;
    view_ray.position = position;
    Matrix transform = MatrixMultiply(MatrixTranslate(0, 0, 1), MatrixRotateXYZ(Vector3(PI * 2 - rotation.y, PI * 2 - rotation.x, 0.0f)));
    view_ray.direction.x = position.x - transform.m12;
    view_ray.direction.y = position.y - transform.m13;
    view_ray.direction.z = position.z - transform.m14;
    view_ray.direction = ((Vector3)view_ray.direction - position).normalised();

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

        if (INPUT_EVENT_ATTACK.isTriggered()) {
            looking_at_block->type = Block::TYPE::AIR;
            looking_at_block->subchunk->generateMesh();
        }

        if (INPUT_EVENT_INTERACT.isJustTriggered()) {
            OS::print("Block position: " + Vector3(looking_at_block->x, looking_at_block->y, looking_at_block->z).toString());
            OS::print("Face: " + directionToString(looking_at_face));

            const float* pos = dGeomGetPosition(looking_at_block->subchunk->getShape());
            const float* off = dGeomGetOffsetPosition(looking_at_block->subchunk->getShape());

            OS::print("Geom pos: " + Vector3(pos[0] + off[0], pos[1] + off[1], pos[2] + off[2]).toString());
            OS::print("Subchunk pos: " + looking_at_block->subchunk->getGlobalPosition().toString());
            OS::print("Subchunk id: " + to_string(looking_at_block->subchunk->getId()));
            OS::print("-------------------");
        }
    }

}

void Player::draw() {
    if (looking_at_block) {
        #define SIZE 1.01f
        Draw::drawBoundingBox(looking_at_block->getBoundingBox(), Colour(0.9, 0.9, 0.9), Vector3::ZERO(), Vector3(SIZE, SIZE, SIZE));
    }

    #define CROSSHAIR_SIZE 10
    Vector2 center = OS::getScreenSize() / 2.0;
    Draw::drawLine(center - Vector2(CROSSHAIR_SIZE, 0), center + Vector2(CROSSHAIR_SIZE, 0), Colour::BLACK());
    Draw::drawLine(center - Vector2(0, CROSSHAIR_SIZE), center + Vector2(0, CROSSHAIR_SIZE), Colour::BLACK());
}

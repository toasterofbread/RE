#include "player.h"

#include "chunk.h"

#include "common/draw.h"
#include "common/enums.h"
#include "common/input.h"
#include "core/resource/shape/box_shape.h"

#define MOUSE_SENSITIVITY 0.20f
#define MOVEMENT_SPEED_H 9.0f
#define MOVEMENT_SPEED_V 5.0f
#define CAMERA_Y_CLAMP_MARGIN 0.0001f
#define BLOCK_REACH_DISTANCE numeric_limits<float>::max()

void Player::ready() {
    super::ready();
 
    setPosition(Vector3(10, CHUNK_HEIGHT, 10));
    setFixedRotation(true);
    setGravityScale(Vector3::ZERO());

    auto box = Shape::create<BoxShape3D>();
    box->setSize(Vector3(0.5f, 1.8f, 0.5f));
    shape.setShape(box);
    addChild(&shape);

    addChild(&camera);
    camera.enable();

    addChild(&hotbar);

    INPUT_EVENT_ATTACK.hold_type = InputEvent::HOLD_TYPE::REPEAT;
    INPUT_EVENT_ATTACK.hold_interval = 0.1f;

    INPUT_EVENT_INTERACT.hold_type = InputEvent::HOLD_TYPE::REPEAT;
    INPUT_EVENT_INTERACT.hold_interval = 0.1f;
}

void Player::process(float delta) {
    super::process(delta);

    // Get the rotation delta from mouse movement or left joystick
    Vector2 rotation_delta = Vector2(GetMouseDelta());
    if (rotation_delta.isZero())
        rotation_delta = Input::getAnalogPadVector(SIDE::RIGHT).normalised() * 10.0f * delta;

    // Apply rotation delta to the final rotation
    camera_angle.x -= rotation_delta.x * MOUSE_SENSITIVITY;
    camera_angle.y = clamp(camera_angle.y - rotation_delta.y * delta * MOUSE_SENSITIVITY, -PI / 2 + CAMERA_Y_CLAMP_MARGIN, PI / 2 - CAMERA_Y_CLAMP_MARGIN);
    camera.setRotation(QuaternionFromEuler(camera_angle.y, camera_angle.x, 0));

    Vector3 position = getPosition();

    // Apply DPad or joystick value to movement
    Vector2 h_movement = Input::getPadVector();
    if (h_movement.isZero())
        h_movement = Input::getAnalogPadVector(SIDE::LEFT);
    h_movement.normalise();

    Vector3 velocity;
    // if (!h_movement.isZero())
        velocity = Vector3(h_movement.x, 0.0f, h_movement.y).rotatedByQuat(camera.getGlobalRotation().getAxisComponent(Vector3::UP())) * MOVEMENT_SPEED_H;
    velocity.y = getLinearVelocity().y;

    // Vertical movement
    int v_movement = INPUT_EVENT_JUMP.isTriggered() - INPUT_EVENT_CROUCH.isTriggered();
    // if (v_movement != 0)
        velocity.y = v_movement * MOVEMENT_SPEED_V;

    // Apply final velocity
    // if (!velocity.isZero()) {
        setLinearVelocity(velocity);
        getBody()->activate();
    // }

    OS::dbPrint("Camera rotation: " + camera.getRotation().toString());
    OS::dbPrint("Player position: " + getPosition().toString());
    OS::dbPrint("Player velocity: " + velocity.toString());

    // Get camera view ray
    Ray view_ray = {position, (camera.getCameraTarget() - camera.getCameraPosition()).normalised()};

    // Get list of subchunks intersecting view ray, in order of proximity to player
    list<pair<SubChunk*, float>> intersecting;
    Chunk* chunk = World::get()->chunks;
    while (chunk) {
        
        // Check if view ray intersects chunk bounding box
        RayCollision chunk_collision = GetRayCollisionBox(view_ray, chunk->bounding_box);
        if (chunk_collision.hit && chunk_collision.distance <= BLOCK_REACH_DISTANCE) {

            // Iterate through subchunks
            for (int i = 0; i < SUBCHUNK_COUNT; i++) {
                SubChunk* sub = chunk->sub_chunks[i];

                // Check if view ray intersects subchunk bounding box
                RayCollision subchunk_collision = GetRayCollisionBox(view_ray, sub->bounding_box);
                if (subchunk_collision.hit && subchunk_collision.distance <= BLOCK_REACH_DISTANCE) {

                    // First colliding subchunk can go directly into list
                    if (intersecting.empty()) {
                        intersecting.push_back(make_pair(sub, subchunk_collision.distance));
                    }
                    else {
                        // Find the first intersected subchunk further away than this one
                        bool found = false;
                        for (auto i = intersecting.begin(); i != intersecting.end(); i++) {
                            if (subchunk_collision.distance < i->second) {
                                intersecting.insert(i, make_pair(sub, subchunk_collision.distance));
                                found = true;
                                break;
                            }
                        }

                        // If no further subchunk was found, this one is the furthest away
                        if (!found)
                            intersecting.push_back(make_pair(sub, subchunk_collision.distance));
                    }
                }
            }
        }

        chunk = chunk->next;
    }

    looking_at_block = NULL;
    DIRECTION_3 previous_looking_at_face = looking_at_face;
    looking_at_face = DIRECTION_3::NONE;

    // Iterate through intersected subchunks
    BoundingBox box;
    for (auto i : intersecting) {

        SubChunk* chunk = i.first;
        Vector3 chunk_position = chunk->chunk->getGlobalPosition();
        chunk_position.y = 0;

        RayCollision closest_collision = { false };

        // Iterate through subchunk blocks
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = SUBCHUNK_HEIGHT * chunk->index; y < SUBCHUNK_HEIGHT * (chunk->index + 1); y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    Block* block = chunk->chunk->getBlock(x, y, z);
                    if (block == NULL || !block->isTangible()) {
                        continue;
                    }

                    // Check if view ray intersects with block
                    RayCollision block_collision = block->checkInteractionRay(view_ray, chunk_position);
                    
                    // Overwrite looking_at_block if block is closer
                    if (block_collision.hit && block_collision.distance <= BLOCK_REACH_DISTANCE && (!closest_collision.hit || block_collision.distance < closest_collision.distance)) {
                        looking_at_block = block;
                        closest_collision = block_collision;
                    }
                }
            }
        }

        // End iteration if block was found
        if (looking_at_block) {
            
            looking_at_face = DIRECTION_3::NONE;
            
            for (int i = 0; i < 3; i++) {
                float normal = ((Vector3)closest_collision.normal)[i];
                if (abs((int)normal) == 1) {
                    switch (i) {
                        case 0: looking_at_face = normal == 1 ? DIRECTION_3::RIGHT : DIRECTION_3::LEFT; break;
                        case 1: looking_at_face = normal == 1 ? DIRECTION_3::UP : DIRECTION_3::DOWN; break;
                        case 2: looking_at_face = normal == 1 ? DIRECTION_3::BACK : DIRECTION_3::FRONT; break;
                    }
                    break;
                }
                // Handle the normal sometimes being incorrect
                else if (normal != 0.0f) {
                    looking_at_face = previous_looking_at_face;
                    break;
                }
            }

            Draw::markPoint(closest_collision.point, Colour::BLUE());
            break;
        }
    }

    if (looking_at_block != NULL) {
        
        if (INPUT_EVENT_PICKBLOCK.isJustTriggered()) {
            
        }

        if (INPUT_EVENT_INTERACT.isJustTriggered()) {
            if (INPUT_EVENT_CROUCH.isTriggered()) {
                if (looking_at_block->isInteractable())
                    looking_at_block->interact(this);
            }
            else {
                Block* place_block = looking_at_block->getNeighbour(looking_at_face);
                if (place_block == NULL) {
                    OS::print("INV: " + directionToString(looking_at_face));
                }
                else if (hotbar.getSelected() != Block::TYPE::INVALID) {
                    place_block->setType(hotbar.getSelected());
                    place_block->subchunk->requestMeshGeneration();
                }
            }
        }

        if (INPUT_EVENT_ATTACK.isJustTriggered()) {
            looking_at_block->setType(Block::TYPE::AIR);
            looking_at_block->subchunk->requestMeshGeneration();
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

#include "chunk.h"

#include "world.h"
#include "player.h"

#include "common/draw.h"
#include "node/types/camera_3d.h"

#include <btBulletCollisionCommon.h>

void SubChunk::setup(Chunk* _chunk, int _index) {
    chunk = _chunk;
    index = _index;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                Block* block = chunk->getBlock(x, y, z);
                if (block == NULL)
                    return;
                block->subchunk = this;
                // collision_shape->addShape(x, y, z, block->getTypeCollisionShape());
            }
        }
    }

    Draw::loadStep("Generating chunkmeshes...");
    generateBoundingBox();
    generateMesh();
    onMeshGenerationFinished();
    setShape(collision_shape);
}

Vector3 SubChunk::getCenter() {
    return Vector3((chunk->getGridPos().x + 0.5f) * CHUNK_SIZE, (index + 0.5f) * SUBCHUNK_HEIGHT, (chunk->getGridPos().y + 0.5f) * CHUNK_SIZE);
}

void Chunk::setup(Vector2 grid_pos, Chunk* chunks[CHUNK_AMOUNT][CHUNK_AMOUNT]) {

    setType(TYPE::STATIC);
    setGravityScale(Vector3::ZERO());
    setLinearVelocity(Vector3::ZERO());

    setEnabled(false);

    grid_position = grid_pos;
    setPosition(Vector3((grid_pos * CHUNK_SIZE).x, 0, (grid_pos * CHUNK_SIZE).y));

    setNeighbour(DIRECTION::UP, grid_position.y < CHUNK_AMOUNT - 1 ? chunks[(int)grid_position.x][(int)grid_position.y + 1] : NULL);
    setNeighbour(DIRECTION::DOWN, grid_position.y > 0 ? chunks[(int)grid_position.x][(int)grid_position.y - 1] : NULL);
    setNeighbour(DIRECTION::LEFT, grid_position.x > 0 ? chunks[(int)grid_position.x - 1][(int)grid_position.y] : NULL);
    setNeighbour(DIRECTION::RIGHT, grid_position.x < CHUNK_AMOUNT - 1 ? chunks[(int)grid_position.x + 1][(int)grid_position.y] : NULL);

    bounding_box.min = Vector3(grid_position.x * CHUNK_SIZE, 0.0f, grid_position.y * CHUNK_SIZE);
    bounding_box.max = Vector3((grid_position.x + 1) * CHUNK_SIZE, CHUNK_HEIGHT, (grid_position.y + 1) * CHUNK_SIZE);

    Vector3 position = getPosition();
    for (int i = 0; i < SUBCHUNK_COUNT; i++) {
        SubChunk* sub = new SubChunk;
        sub->setPosition(Vector3(0.0f, i * SUBCHUNK_HEIGHT, 0.0f));
        addChild(sub);
        sub_chunks[i] = sub;
    }

    for (int i = 0; i < SUBCHUNK_COUNT; i++) {
        sub_chunks[i]->setup(this, i);
    }
}

void Chunk::createBlocks() {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z] = new Block(this, x, y, z);
            }
        }
    }
}

Block* Chunk::getBlock(int x, int y, int z) {
    
    if (x < 0 || x >= CHUNK_SIZE)
        return NULL;
    
    if (y < 0 || y >= CHUNK_HEIGHT)
        return NULL;
    
    if (z < 0 || z >= CHUNK_SIZE)
        return NULL;
    
    return blocks[x][y][z];
}

Block* Chunk::getBlock(Vector3 position) {
    return getBlock(position.x, position.y, position.z);
}

Chunk* Chunk::getNeighbour(DIRECTION direction) {
    switch (direction) {
        case DIRECTION::UP: return front;
        case DIRECTION::DOWN: return back;
        case DIRECTION::LEFT: return left;
        case DIRECTION::RIGHT: return right;
        default: return NULL;
    }
}

void Chunk::setNeighbour(DIRECTION direction, Chunk* chunk) {
    Chunk** neighbour;
    switch (direction) {
        case DIRECTION::UP: neighbour = &front; break;
        case DIRECTION::DOWN: neighbour = &back; break;
        case DIRECTION::LEFT: neighbour = &left; break;
        case DIRECTION::RIGHT: neighbour = &right; break;
        default: return;
    }

    if (*neighbour == chunk)
        return;

    if (*neighbour != NULL)
        (*neighbour)->SIGNAL_ENTITY_CONTAINED_CHANGED.disconnect(this);

    *neighbour = chunk;

    if (chunk != NULL)
        chunk->SIGNAL_ENTITY_CONTAINED_CHANGED.connect(this, &Chunk::onNeighbourEntityContainedChanged, false, chunk);
}

void Chunk::onNeighbourEntityContainedChanged(Chunk* neighbour) {
    bool enable = false;
    for (int dir = 0; dir < DIRECTION_COUNT; dir++) {
        Chunk* neighbour = getNeighbour((DIRECTION)dir);
        if (neighbour == NULL)
            continue;
        if (neighbour->getContainedEntityCount() > 0) {
            enable = true;
            break;
        }
    }
    setEnabled(enable);
}

void Chunk::onEntityEntered(Entity* entity) {
    contained_entities.push_back(entity);

    if (contained_entities.size() == 1) {
        SIGNAL_ENTITY_CONTAINED_CHANGED.emit();
        setEnabled(true);
    }
}

void Chunk::onEntityExited(Entity* entity) {
    contained_entities.remove(entity);

    if (contained_entities.empty()) {
        SIGNAL_ENTITY_CONTAINED_CHANGED.emit();

        bool enable = false;
        for (int dir = 0; dir < DIRECTION_COUNT; dir++) {
            Chunk* neighbour = getNeighbour((DIRECTION)dir);
            if (neighbour == NULL)
                continue;
            if (neighbour->getContainedEntityCount() > 0) {
                enable = true;
                break;
            }
        }
        setEnabled(enable);
    }
}

Chunk* Chunk::getFront() {
    return front;
}
Chunk* Chunk::getBack() {
    return back;
}
Chunk* Chunk::getLeft() {
    return left;
}
Chunk* Chunk::getRight() {
    return right;
}

int Chunk::getContainedEntityCount() {
    return contained_entities.size();
}

void SubChunk::requestMeshGeneration() {
    World::get()->requestSubChunkMeshGeneration(this);
    OS::print("GEN: " + to_string(index));
}

int SubChunk::getAxisLength(int axis) {
    switch (axis) {
        case 0:
        case 2: return CHUNK_SIZE;
        case 1: return SUBCHUNK_HEIGHT;
    }
    ASSERT(false);
    return 0;
}

#define ITERATE_BLOCKS(code) \
for (int x = 0; x < CHUNK_SIZE; x++) { \
    for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) { \
        for (int z = 0; z < CHUNK_SIZE; z++) { \
            code; \
        } \
    } \
}

Block::TYPE Chunk::getBlockType(int x, int y, int z) {
    Block* block = getBlock(x, y, z);
    if (block)
        return block->getType();
    return Block::TYPE::INVALID;
}

Block::TYPE Chunk::getBlockType(Vector3 position) {
    return getBlockType(position.x, position.y, position.z);
}

// Greedy meshing code from https://github.com/BLaZeKiLL/UE5VoxelTutorial/blob/master/Source/UE5VoxelTutorial/Private/Voxel/Chunk/GreedyChunk.cpp
void SubChunk::generateMesh() {
    
    mesh.reset();
    
    if (!mesh_loaded) {
        mesh.resize(10002);
    }

    const Vector3 block_offset = Vector3(0, index * SUBCHUNK_HEIGHT, 0);

    for (int axis = 0; axis < 3; ++axis) {

        const int axisA = (axis + 1) % 3;
        const int axisB = (axis + 2) % 3;
        
        const int axis_limit = SubChunk::getAxisLength(axis);
        const int axis_a_limit = SubChunk::getAxisLength(axisA);
        const int axis_b_limit = SubChunk::getAxisLength(axisB);

        Vector3 axis_a_delta = Vector3::ZERO();
        Vector3 axis_b_delta = Vector3::ZERO();
        
        Vector3 chunk_iteration = Vector3::ZERO();

        Vector3 axis_mask = Vector3::ZERO();
        axis_mask[axis] = 1;

        int mask[axis_a_limit * axis_b_limit];

        for (chunk_iteration[axis] = -1; chunk_iteration[axis] < axis_limit;) {
            int n = 0;
            
            for (chunk_iteration[axisB] = 0; chunk_iteration[axisB] < axis_b_limit; chunk_iteration[axisB]++) {
                for (chunk_iteration[axisA] = 0; chunk_iteration[axisA] < axis_a_limit; chunk_iteration[axisA]++) {
                    const Block::TYPE current_block = chunk->getBlockType(chunk_iteration + block_offset);
                    const Block::TYPE compare_block = chunk->getBlockType(chunk_iteration + axis_mask + block_offset);

                    const bool current_block_opaque = current_block != Block::TYPE::AIR && current_block != Block::TYPE::INVALID;
                    const bool compare_block_opaque = compare_block != Block::TYPE::AIR && compare_block != Block::TYPE::INVALID;
                    
                    if (current_block_opaque == compare_block_opaque) {
                        mask[n++] = (int)Block::TYPE::INVALID;
                    }
                    else if (current_block_opaque) {
                        mask[n++] = (int)current_block;
                    }
                    else {
                        mask[n++] = -(int)compare_block;
                    }
                }
            }

            chunk_iteration[axis]++;
            n = 0;

            for (int j = 0; j < axis_b_limit; ++j) {
                for (int i = 0; i < axis_a_limit;) {
                    if (mask[n] != (int)Block::TYPE::INVALID) {
                        const int currentMask = mask[n];
                        chunk_iteration[axisA] = i;
                        chunk_iteration[axisB] = j;
                        
                        int width = 1;
                        while (i + width < axis_a_limit && mask[n + width] == currentMask) {
                            width++;
                        }
                        
                        int height;
                        bool done = false;

                        for (height = 1; j + height < axis_b_limit; height++) {
                            for (int k = 0; k < width; ++k) {
                                if (mask[n + k + height * axis_a_limit] != currentMask) {
                                    done = true;
                                    break;
                                }
                            }
                            if (done)
                                break;
                        }

                        axis_a_delta[axisA] = width;
                        axis_b_delta[axisB] = height;
                        
                        mesh.addFace(
                            chunk_iteration,
                            chunk_iteration + axis_a_delta,
                            chunk_iteration + axis_b_delta,
                            chunk_iteration + axis_a_delta + axis_b_delta,
                            sign(currentMask) * axis_mask, sign(currentMask), (Block::TYPE)abs(currentMask), width, height
                        );

                        axis_a_delta = Vector3::ZERO();
                        axis_b_delta = Vector3::ZERO();
                        
                        for (int l = 0; l < height; ++l) {
                            for (int k = 0; k < width; ++k) {
                                mask[n + k + l * axis_a_limit] = (int)Block::TYPE::INVALID;
                            }
                        }
                        
                        i += width;
                        n += width;
                    }
                    else {
                        i++;
                        n++;
                    }
                }
            }
        }
    }
    
    if (!mesh_loaded) {
        mesh.upload();
        mesh_loaded = true;
    }
    else {
        mesh.updateBuffer();
    }
}

void SubChunk::onMeshGenerationFinished() {
}

void SubChunk::generateBoundingBox() {
    bounding_box.min = Vector3(chunk->getGridPos().x * CHUNK_SIZE, index * SUBCHUNK_HEIGHT, chunk->getGridPos().y * CHUNK_SIZE);
    bounding_box.max = Vector3((chunk->getGridPos().x + 1) * CHUNK_SIZE, (index + 1) * SUBCHUNK_HEIGHT, (chunk->getGridPos().y + 1) * CHUNK_SIZE);
}

void SubChunk::drawChunk() {

    if (drawn)
        return;
    drawn = true;

    if (!isGlobalVisible())
        return;

    Vector3 position = chunk->getGlobalPosition();

    if (!isVisible(chunk->getTree()->getEnabledCamera3D(), position)) {
        return;
    }

    Chunk* entered = World::get()->getPlayer()->getEnteredChunk();
    if (entered != NULL && entered->sub_chunks[SUBCHUNK_COUNT - 1] == this) {
        Draw::drawCube(getCenter() + Vector3(0.0f, CHUNK_SIZE * 0.5f, 0.0f), Vector3::ONE(), Colour::RED());
    }

    Draw::drawMesh(mesh, World::get()->material, MatrixTranslate(position.x, position.y - 1 + (index * SUBCHUNK_HEIGHT), position.z));
}

bool SubChunk::isVisible(Camera3D* camera, Vector3 chunk_position) {

    return true;

    Camera3D::Frustum& frustum = camera->getFrustum();

    for (int face = 0; face < Camera3D::Frustum::FACE_COUNT - 1; face++) {

        const float r = CHUNK_SIZE * 0.5 * abs(frustum.faces[face].normal.x) +
                SUBCHUNK_HEIGHT * 0.5 * abs(frustum.faces[face].normal.y) + CHUNK_SIZE * 0.5 * abs(frustum.faces[face].normal.z);

        if (frustum.faces[face].getDistanceTo(Vector3Transform(chunk_position + Vector3(CHUNK_SIZE, (index + 1) * SUBCHUNK_HEIGHT, CHUNK_SIZE) * 0.5, GetCameraMatrix(*camera->getCamera()))) < -r) {
            return false;
        }
    }

    return true;
}

bool SubChunk::isAxisOpaque(AXIS axis) {

    switch (axis) {
        case AXIS::X: {
            for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    bool opaque = false;
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;;
        }
        case AXIS::Y: {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    bool opaque = false;
                    for (int y = SUBCHUNK_HEIGHT * index; x < SUBCHUNK_HEIGHT * (index + 1); x++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;
        }
        default: { // Z
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
                    bool opaque = false;
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;
        }
    }
}

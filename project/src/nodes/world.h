#ifndef INCLUDED_WORLD
#define INCLUDED_WORLD

#include "node/types/node_3d.h"
#include "common/vector2.h"
#include "physics/node/collision_shape_3d.h"

#define CHUNK_AMOUNT 5
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 16 * 4
#define SUBCHUNK_HEIGHT 16
#define SUBCHUNK_COUNT (CHUNK_HEIGHT / SUBCHUNK_HEIGHT)

// Forward declarations
class Player;
class World;
class Block;
class Chunk;

struct SubChunk {
    Chunk* chunk;
    int index;
    Mesh mesh;

    bool drawn = false;

    SubChunk(Chunk* _chunk, int _index);
    bool isVisible(Camera3D* camera, Vector3 chunk_position);

    bool isAxisOpaque(AXIS axis);
    void draw();

    BoundingBox bounding_box;

    Vector3 getCenter();
    void generateMesh();

    private:
        void generateBoundingBox();
        void addFace(DIRECTION_3 dir, int x, int y, int z, int face_i);
        bool mesh_loaded = false;
        unsigned int allocated_vertices = 0;
};

struct Chunk: public Node3D {
    REGISTER_NODE(Chunk, Node3D, {});

    Chunk* front;
    Chunk* back;
    Chunk* left;
    Chunk* right;

    Chunk* next = NULL;
    Chunk* previous = NULL;

    Block* blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    Block* getBlock(int x, int y, int z, bool allow_nonexistent = false);

    World* world;

    void setup(Vector2 grid_pos, Chunk* chunks[CHUNK_AMOUNT][CHUNK_AMOUNT]);

    Vector2 getGridPos() { return grid_position; }
    SubChunk* sub_chunks[SUBCHUNK_COUNT];

    BoundingBox bounding_box;

    private:
        Vector2 grid_position;
        Matrix transform;
        bool mesh_loaded = false;
};

struct Block {

    unsigned int x;
    unsigned int y;
    unsigned int z;
    
    bool transparent = false;
    bool exists = true;

    Chunk* chunk;
    SubChunk* subchunk;
    Block(Chunk* _chunk, unsigned int _x, unsigned int _y, unsigned int _z) {
        chunk = _chunk;
        x = _x;
        y = _y;
        z = _z;
    }

    Block* get(DIRECTION_3 dir) {
        switch (dir) {
            case DIRECTION_3::UP: return getUp();
            case DIRECTION_3::DOWN: return getDown();
            case DIRECTION_3::LEFT: return getLeft();
            case DIRECTION_3::RIGHT: return getRight();
            case DIRECTION_3::FRONT: return getFront();
            case DIRECTION_3::BACK: return getBack();
        }
        return NULL;
    }

    void getBoundingBox(BoundingBox* box, Vector3 chunk_position) {
        chunk_position.y = 0;
        box->min = chunk_position + Vector3(x, y, z);
        box->max = chunk_position + Vector3(x + 1, y + 1, z + 1);
    }

    BoundingBox getBoundingBox() {
        BoundingBox ret;
        getBoundingBox(&ret, chunk->getGlobalPosition());
        return ret;
    }

    Block* getLeft() {
        if (x > 0)
            return chunk->getBlock(x - 1, y, z);
        return NULL;
    }
    Block* getRight() {
        if (x < CHUNK_SIZE - 1)
            return chunk->getBlock(x + 1, y, z);
        return NULL;
    }

    Block* getDown() {
        if (y > 0)
            return chunk->getBlock(x, y - 1, z);
        return NULL;
    }
    Block* getUp() {
        if (y < CHUNK_HEIGHT - 1)
            return chunk->getBlock(x, y + 1, z);
        return NULL;
    }

    Block* getFront() {
        if (z > 0)
            return chunk->getBlock(x, y, z - 1);
        return NULL;
    }
    Block* getBack() {
        if (z < CHUNK_SIZE - 1)
            return chunk->getBlock(x, y, z + 1);
        return NULL;
    }
};

class World: public Node3D {

    public:
        REGISTER_NODE_WITH_CONSTRUCTOR(World, Node3D, {}, init());

        void draw();

        void loadChunk(Vector2 grid_position);
        void unloadChunk(Vector2 grid_position);

        int chunk_count = CHUNK_AMOUNT * CHUNK_AMOUNT;
        Chunk* chunks = NULL;
        Material material;

        SubChunk* starting_chunk = NULL;

        Chunk* getChunk(int x, int y, bool grid_pos = true);

        void addSubChunk(SubChunk* chunk);

    private:
        Player* player;
        void init();

};

#endif
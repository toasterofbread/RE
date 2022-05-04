#ifndef INCLUDED_WORLD
#define INCLUDED_WORLD

#include "node/types/node_3d.h"
#include "common/vector2.h"
#include "physics/node/collision_shape_3d.h"
#include "physics/node/physics_body_3d.h"

#include <btBulletCollisionCommon.h>
#include <thread>
using namespace std;

#define CHUNK_AMOUNT 10
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 16 * 1
#define SUBCHUNK_HEIGHT 16
#define SUBCHUNK_COUNT (CHUNK_HEIGHT / SUBCHUNK_HEIGHT)
#define TEXTURE_MAP_WIDTH 16.0
#define TEXTURE_MAP_HEIGHT 16.0

#define CHUNKMESH_GENERATION_THREADS 1
static_assert(CHUNKMESH_GENERATION_THREADS >= 0);

// Forward declarations
class Player;
class World;
class Block;
class Chunk;

struct SubChunk: public CollisionShape3D  {

    REGISTER_NODE(SubChunk, CollisionShape3D, {});

    Chunk* chunk;
    int index;
    Mesh mesh;

    bool drawn = false;

    void init(Chunk* _chunk, int _index);
    bool isVisible(Camera3D* camera, Vector3 chunk_position);

    bool isAxisOpaque(AXIS axis);
    void draw();

    BoundingBox bounding_box;

    Vector3 getCenter();
    void requestMeshGeneration();

    private:
        void generateBoundingBox();
        void addFace(DIRECTION_3 face, Block* block, int face_i);
        bool mesh_loaded = false;
        unsigned int allocated_vertices = 0;
        bool pending_generation = false;

        friend class World;
        void generateMesh();
        void onMeshGenerationFinished();
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
    void createBlocks();

    Vector2 getGridPos() { return grid_position; }
    SubChunk* sub_chunks[SUBCHUNK_COUNT];

    BoundingBox bounding_box;

    private:
        Vector2 grid_position;
        Matrix transform;
        bool mesh_loaded = false;
};

struct Block {

    // Chunk-relative coordinates
    int x; int y; int z;

    // Global coordinates
    int global_x; int global_y; int global_z;

    enum class TYPE {
        AIR, GRASS, DIRT, STONE
    };
    #define BLOCK_TYPE_COUNT ((int)TYPE::STONE + 1)
    TYPE type = TYPE::GRASS;

    struct TypeData {
        bool populated = false;
        bool tangible = true;
        int texcoords[DIRECTION_3_COUNT][2];

        TypeData() {
            for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                texcoords[face][0] = -1;
                texcoords[face][1] = -1;
            }
        }
    };
    static TypeData type_data[BLOCK_TYPE_COUNT];
    static void loadTypeData();

    static string getTypeName(TYPE type) {
        switch (type) {
            case TYPE::AIR: return "air";
            case TYPE::GRASS: return "grass";
            case TYPE::DIRT: return "dirt";
            case TYPE::STONE: return "stone";
            default: return "invalid type";
        }
    }

    static TYPE getTypeFromString(string name) {
        name = lowerString(name);
        for (int type = 0; type < BLOCK_TYPE_COUNT; type++) {
            if (name == getTypeName((TYPE)type)) {
                return (TYPE)type;
            }
        }
        return (TYPE)-1;
    }

    Chunk* chunk;
    SubChunk* subchunk;
    Block(Chunk* _chunk, int _x, int _y, int _z) {
        chunk = _chunk;
        x = _x;
        y = _y;
        z = _z;

        Vector3 chunk_position = chunk->getPosition();
        global_x = x + chunk_position.x;
        global_y = y + chunk_position.y;
        global_z = z + chunk_position.z;
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

    Vector3 getCenter() {
        return Vector3(global_x + 0.5f, global_y + 0.5f, global_z + 0.5f);
    }

    bool isTransparent() {
        return false;
    }

    bool isBlock() {
        return type != TYPE::AIR;
    }

    TYPE getType() {
        return type;
    }

    DIRECTION_3 getNearestFace(Vector3 point) {
        #define MARGIN 0.00001f
        if (point.x <= global_x + MARGIN) {
            return DIRECTION_3::LEFT;
        }
        else if (point.x >= global_x + 1.0 - MARGIN) {
            return DIRECTION_3::RIGHT;
        }
        else if (point.y <= global_y + MARGIN) {
            return DIRECTION_3::DOWN;
        }
        else if (point.y >= global_y + 1.0 - MARGIN) {
            return DIRECTION_3::UP;
        }
        else if (point.z <= global_z + MARGIN) {
            return DIRECTION_3::FRONT;
        }
        else if (point.z >= global_z + 1.0 - MARGIN) {
            return DIRECTION_3::BACK;
        }
        else {
            return DIRECTION_3::NONE;
        }
    }

    int* getFaceTexcoords(DIRECTION_3 face) {
        return type_data[(int)getType()].texcoords[(int)face];
    }

    void getBoundingBox(BoundingBox* box, Vector3 chunk_position) {
        chunk_position.y = -1;
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
        else if (chunk->left)
            return chunk->left->getBlock(CHUNK_SIZE - 1, y, z);
        return NULL;
    }
    Block* getRight() {
        if (x < CHUNK_SIZE - 1)
            return chunk->getBlock(x + 1, y, z);
        else if (chunk->right)
            return chunk->right->getBlock(0, y, z);
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
        // else if (chunk->front)
        //     return chunk->front->getBlock(x, y, CHUNK_SIZE - 1);
        return NULL;
    }
    Block* getBack() {
        if (z < CHUNK_SIZE - 1)
            return chunk->getBlock(x, y, z + 1);
        // else if (chunk->back)
        //     return chunk->back->getBlock(x, y, 0);
        return NULL;
    }
};

struct Thread {

    Signal<> SIGNAL_FINISHED;

    template<typename ObjectType, typename ReturnType, typename... Arguments>
    void set(ObjectType* object, ReturnType(ObjectType::*function)(Arguments...), Arguments... arguments) {
        if (caller != NULL) {
            delete caller;
        }
        caller = new Caller(object, function, arguments...);
        caller->parent = this;
    }

    bool isRunning() { 
        return caller != NULL && caller->running; 
    }

    void start() {
        ASSERT(caller != NULL);
        caller->start();
    }

    void join() {
        caller->join();
    }

    private:
        struct CallerBase {
            Thread* parent;
            bool running = false;
            virtual void start() = 0;
            virtual void join() = 0;
        };

        template<typename ObjectType, typename ReturnType, typename... Arguments>
        struct Caller: public CallerBase {
            ObjectType* object;
            ReturnType(ObjectType::*function)(Arguments...);
            tuple<Arguments...> arguments;
            
            Caller(ObjectType* obj, ReturnType(ObjectType::*func)(Arguments...), Arguments... args): arguments(forward<Arguments>(args)...) {
                object = obj;
                function = func;
            }

            void start() {
                ASSERT(!running);
                _thread = thread(&Caller::threadFunction, this);
            }

            void join() {
                _thread.join();
            }

            private:
                thread _thread;

                template<size_t... I>
                void call(index_sequence<I...>) {
                    (object->*function)(get<I>(arguments)...);
                }

                void threadFunction() {
                    running = true;
                    call(make_index_sequence<sizeof...(Arguments)>{});
                    running = false;
                    parent->SIGNAL_FINISHED.emit();
                }
        };

        CallerBase* caller = NULL;
};

class World: public PhysicsBody3D {

    public:
        REGISTER_NODE_WITH_CONSTRUCTOR(World, PhysicsBody3D, {}, init());

        void draw();

        void loadChunk(Vector2 grid_position);
        void unloadChunk(Vector2 grid_position);

        int chunk_count = CHUNK_AMOUNT * CHUNK_AMOUNT;
        Chunk* chunks = NULL;
        Material material;

        SubChunk* starting_chunk = NULL;

        Chunk* getChunk(int x, int y, bool grid_pos = true);

        void addSubChunk(SubChunk* chunk);

        void requestSubChunkMeshGeneration(SubChunk* chunk);

    private:
        Player* player;
        void init();

        Thread gen_threads[CHUNKMESH_GENERATION_THREADS];
};

#endif
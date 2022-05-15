#ifndef INCLUDED_WORLD
#define INCLUDED_WORLD

#include "node/types/node_3d.h"
#include "common/vector2.h"
#include "physics/node/collision_object_3d.h"
#include "physics/node/physics_body_3d.h"
#include "core/resource/shape/grid_shape.h"
#include "core/resource/shape/box_shape.h"

#include <btBulletCollisionCommon.h>
#include <thread>
using namespace std;

#define CHUNK_AMOUNT 10
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 16 * 2
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
class SubChunk;

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

class World: public Node3D {

    public:
        REGISTER_NODE(World, Node3D, {});

        static World* get() { ASSERT(singleton != NULL); return singleton; }

        void draw();

        void loadChunk(Vector2 grid_position);
        void unloadChunk(Vector2 grid_position);

        int chunk_count = CHUNK_AMOUNT * CHUNK_AMOUNT;
        Chunk* chunks = NULL;
        Material material;

        Chunk* getChunk(int x, int y, bool grid_pos = true);
        void addSubChunk(SubChunk* chunk);

        Player* getPlayer();

        void requestSubChunkMeshGeneration(SubChunk* chunk);

    private:
        static World* singleton;

        Player* player;
        void init();

        Thread gen_threads[CHUNKMESH_GENERATION_THREADS];
};

#endif
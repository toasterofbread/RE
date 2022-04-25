#ifndef INCLUDED_WORLD
#define INCLUDED_WORLD

#include "node/types/node_3d.h"

#define RENDER_DISTANCE 5
#define CHUNK_SIZE 16

// Forward declarations
class Player;

class World: public Node3D {

    public:
        REGISTER_NODE(World, Node3D, {});

        void ready();

        void loadChunk(Vector2 grid_position);
        void unloadChunk(Vector2 grid_position);

    private:

        Player* player;

        struct Chunk: public Node3D {
            REGISTER_NODE(Chunk, Node3D, {});

            void draw();

            void setup(Vector2 grid_pos);

            Chunk* next = NULL;
            Chunk* previous = NULL;

            bool isVisible(Camera3D* camera, Vector3 global_position);
            Vector2 getGridPos() { return grid_position; }

            private:
                Vector2 grid_position;
        };

        Chunk* chunks;

};

#endif
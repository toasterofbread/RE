#ifndef INCLUDED_ENTITY
#define INCLUDED_ENTITY

#include "physics/node/physics_body_3d.h"

// Forward declarations
class World;
class Chunk;

class Entity: public PhysicsBody3D {
    public:
        REGISTER_NODE(Entity, PhysicsBody3D, {});

        void process(float delta);

        Chunk* getEnteredChunk();

    private:
        Chunk* entered_chunk = NULL;

};

#endif
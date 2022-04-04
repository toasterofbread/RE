#include "engine/src/raylib_include.h"
#include <box2d/box2d.h>

#include "engine/src/utils.h"

class PhysicsServer {

    public:
        PhysicsServer();
        static PhysicsServer* getSingleton();

        static b2Vec2 world2Phys(Vector2 world_position);
        static float world2Phys(float world_position);

        static Vector2 phys2World(b2Vec2 physics_position);
        static float phys2World(float physics_position);

        void physicsProcess(float delta);

        void setGravity(Vector2 value);
        void setGravity(b2Vec2 value);
        Vector2 getGravity();

        b2Body* createBody(const b2BodyDef* definition);
        void destroyBody(b2Body* body);

    private:
        static PhysicsServer* singleton;

        static const float world_scale;

        b2Vec2 gravity;
        b2World world = b2World(b2Vec2(0.0f, 0.0f));

        float time_step;
        int32 velocity_iterations;
        int32 position_iterations;
};

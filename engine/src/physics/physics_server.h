#include <box2d/box2d.h>

#include "common/utils.h"

class PhysicsServer {

    public:
        PhysicsServer();
        static PhysicsServer* getSingleton();

        static Vector2 world2Phys(Vector2 world_position);
        static float world2Phys(float world_position);

        static Vector2 phys2World(Vector2 physics_position);
        static float phys2World(float physics_position);

        void physicsProcess(float delta);

        void setGravity(Vector2 value);
        Vector2 getGravity();

        b2Body* createBody(const b2BodyDef* definition);
        void destroyBody(b2Body* body);

    private:
        static PhysicsServer* singleton;

        static const float world_scale;

        Vector2 gravity;
        b2World world = b2World(Vector2(0.0f, 0.0f));

        float time_step;
        int32 velocity_iterations;
        int32 position_iterations;
};

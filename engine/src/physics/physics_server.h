#include <raylib-cpp.hpp>
#include <box2d/box2d.h>

class PhysicsServer {

    public:
        PhysicsServer();
        static PhysicsServer* getSingleton();

        void physicsProcess(float delta);

        void setGravity(Vector2 value);
        void setGravity(b2Vec2 value);
        Vector2 getGravity();

        b2Body* createBody(b2BodyType type = b2_staticBody);
        b2Body* createBody(float x_pos, float y_pos, b2BodyType type = b2_staticBody);

    private:
        static PhysicsServer* singleton;
        b2Vec2 gravity;
        b2World world = b2World(b2Vec2(0.0f, 0.0f));

        float time_step;
        int32 velocity_iterations;
        int32 position_iterations;
};

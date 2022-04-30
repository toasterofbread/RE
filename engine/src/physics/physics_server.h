#include "common/utils.h"


#if PHYSICS_2D_ENABLED
#include <box2d/box2d.h>
#endif

#if PHYSICS_3D_ENABLED
#include "physics/node/physics_body_3d.h"
#include <ode/ode.h> 
#endif

class PhysicsServer {

    public:
        PhysicsServer();
        static PhysicsServer* getSingleton();

        #if PHYSICS_2D_ENABLED
        static Vector2 world2Phys2(Vector2 world_position);
        static float world2Phys2(float world_position);

        static Vector2 phys2World2(Vector2 physics_position);
        static float phys2World2(float physics_position);
        
        void setGravity(Vector2 value);
        Vector2 getGravity2();

        b2Body* createBody2(const b2BodyDef* definition);
        void destroyBody2(b2Body* body);
        #endif

        #if PHYSICS_3D_ENABLED
        static Vector3 world2Phys3(Vector3 world_position);
        static float world2Phys3(float world_position);

        static Vector3 phys2World3(Vector3 physics_position);
        static float phys2World3(float physics_position);
        
        void setGravity(Vector3 value);
        Vector3 getGravity3();

        dBodyID createBody3();
        void destroyBody3(dBodyID body);

        static dWorldID getWorld3() { return singleton->world_3d; }
        static dSpaceID getSpace() { return singleton->main_space; }
        #endif

        void physicsProcess(float delta);

    private:
        static PhysicsServer* singleton;

        #if PHYSICS_2D_ENABLED
        b2World world_2d = b2World(Vector2(0.0f, 0.0f));
        static const float world_scale_2d;
        Vector2 gravity_2d;
        #endif

        #if PHYSICS_3D_ENABLED
        dWorldID world_3d;
        dSpaceID main_space;
        dJointGroupID main_group;
        static const float world_scale_3d;
        Vector3 gravity_3d;
        #endif

        float time_step;
        int32 velocity_iterations;
        int32 position_iterations;
};

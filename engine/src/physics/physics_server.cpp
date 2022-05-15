#include "physics_server.h"

#include "common/utils.h"
#include "common/draw.h"
#include "physics/node/collision_object_3d.h"

#include <box2d/box2d.h>
#include <stdlib.h>

PhysicsServer* PhysicsServer::singleton = NULL;

#if PHYSICS_2D_ENABLED
const float PhysicsServer::world_scale_2d = 13.0f;
#endif
#if PHYSICS_3D_ENABLED
const float PhysicsServer::world_scale_3d = 1.0f;
#endif

#include "LinearMath/btIDebugDraw.h"

class GLDebugDrawer : public btIDebugDraw {
    int mode = DBG_DrawAabb;

    public:

        GLDebugDrawer() {}

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
            // drawLine(from, to, fromColor);
        };
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& colour) {
            // Draw::drawLine(from, to, colour);
        };
        void drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {};
        void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) {};
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {};
        void reportErrorWarning(const char* warningString) {};
        void draw3dText(const btVector3& location, const char* textString) {};
        void setDebugMode(int debugMode) { mode = debugMode; };
        int getDebugMode() const { return mode; }
};

PhysicsServer::PhysicsServer() {
    ASSERT(singleton == NULL);
    singleton = this;
    
    #if PHYSICS_2D_ENABLED
    setGravity(Vector2(0.0f, 100.0f));
    #endif

    #if PHYSICS_3D_ENABLED

    btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration;
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);

    btBroadphaseInterface* broadphase = new btDbvtBroadphase;
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver; // btConstraintSolverPoolMt for multithreading

    world_3d = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

    #if PLATFORM != PLATFORM_VITA
    world_3d->setDebugDrawer(new GLDebugDrawer);
    #endif

    // dInitODE2(0);
    // world_3d = dWorldCreate();
    // main_space = dHashSpaceCreate(NULL);
    // main_group = dJointGroupCreate(0);
    
    setGravity(Vector3(0.0f, -9.8f, 0.0f));
    #endif

    time_step = 1.0f;
    velocity_iterations = 8;
    position_iterations = 3;
}

#if PHYSICS_2D_ENABLED
Vector2 PhysicsServer::world2Phys2(Vector2 world_position) {
    return Vector2(world2Phys2(world_position.x), world2Phys2(world_position.y));
}
float PhysicsServer::world2Phys2(float world_position) {
    return world_position / world_scale_2d;
}
Vector2 PhysicsServer::phys2World2(Vector2 physics_position) {
    return Vector2(phys2World2(physics_position.x), phys2World2(physics_position.y));
}
float PhysicsServer::phys2World2(float physics_position) {
    return physics_position * world_scale_2d;
}
#endif

#if PHYSICS_3D_ENABLED
Vector3 PhysicsServer::world2Phys3(Vector3 world_position) {
    return Vector3(world2Phys3(world_position.x), world2Phys3(world_position.y), world2Phys3(world_position.z));
}
float PhysicsServer::world2Phys3(float world_position) {
    return world_position / world_scale_3d;
}
Vector3 PhysicsServer::phys2World3(Vector3 physics_position) {
    return Vector3(phys2World3(physics_position.x), phys2World3(physics_position.y), phys2World3(physics_position.z));
}
float PhysicsServer::phys2World3(float physics_position) {
    return physics_position * world_scale_3d;
}
#endif

PhysicsServer* PhysicsServer::getSingleton() {
    ASSERT(singleton);
    return singleton;
}

// void callNearCallback(void *data, dGeomID o1, dGeomID o2) {
//     PhysicsServer::getSingleton()->nearCallback(data, o1, o2);
// }

// void PhysicsServer::nearCallback(void *data, dGeomID o1, dGeomID o2) {

//     CollisionObject3D* A = reinterpret_cast<CollisionObject3D*>(dGeomGetData(o1));
//     CollisionObject3D* B = reinterpret_cast<CollisionObject3D*>(dGeomGetData(o2));

//     ASSERT(A);
//     ASSERT(B);

//     if (A->getAttachedBody() == B->getAttachedBody()) {
//         return;
//     }

//     dBodyID b1 = dGeomGetBody(o1);
//     dBodyID b2 = dGeomGetBody(o2);
//     // if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
//     //     return;

//     dContact* contacts = (dContact*)malloc(MAX_CONTACTS * sizeof(dContact));
//     contact_pool.push_back(contacts);

//     int i;
//     for (i = 0; i < MAX_CONTACTS; i++) {
//         contacts[i].surface.mode = dContactMu2;
//         contacts[i].surface.mu = dInfinity;
//         contacts[i].surface.mu2 = dInfinity;

//         // contacts[i].surface.mode = dContactSoftERP;
//         // contacts[i].surface.mu = 0.0;
//         // contacts[i].surface.soft_cfm = 0.000001;
//         // contacts[i].surface.soft_erp = 0.000001;
//     }

//     int contact_count = dCollide(o1, o2, MAX_CONTACTS | CONTACTS_UNIMPORTANT, &contacts[0].geom, sizeof(dContact));
    
//     for (i = 0; i < contact_count; i++) {
//         dJointID c = dJointCreateContact(world_3d, main_group, &contacts[i]);
//         dJointAttach(c, b1, b2);

//         CollisionObject3D::Contact* contact = new CollisionObject3D::Contact(&contacts[i], A, B);
//         A->contacts.push_back(contact);
//         B->contacts.push_back(contact);
//     }
// }

void PhysicsServer::physicsProcess(float delta) {

    if (delta <= 0.0f) {
        return;
    }

    #if PHYSICS_2D_ENABLED
    world_2d.Step(time_step * delta, velocity_iterations, position_iterations);
    #endif

    #if PHYSICS_3D_ENABLED
    world_3d->stepSimulation(time_step * delta);

    #if PLATFORM != PLATFORM_VITA
    world_3d->debugDrawWorld();
    #endif
    
    #endif
}

#if PHYSICS_2D_ENABLED
void PhysicsServer::setGravity(Vector2 value) {
    gravity_2d.Set(value.x, value.y);
    world_2d.SetGravity(gravity_2d);
}
Vector2 PhysicsServer::getGravity2() {
    return gravity_2d;
}

b2Body* PhysicsServer::createBody2(const b2BodyDef* definition) {
    return world_2d.CreateBody(definition);
}

void PhysicsServer::destroyBody2(b2Body* body) {
    world_2d.DestroyBody(body);
}
#endif

#if PHYSICS_3D_ENABLED
void PhysicsServer::setGravity(Vector3 value) {
    world_3d->setGravity(PhysicsServer::world2Phys3(value));
}
Vector3 PhysicsServer::getGravity3() {
    return PhysicsServer::phys2World3(world_3d->getGravity());
}

#endif

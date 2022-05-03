#include "physics_server.h"

#include <box2d/box2d.h>
#include <stdlib.h>

#include "common/utils.h"
#include "common/draw.h"
#include "physics/node/collision_shape_3d.h"

PhysicsServer* PhysicsServer::singleton = NULL;

#if PHYSICS_2D_ENABLED
const float PhysicsServer::world_scale_2d = 13.0f;
#endif
#if PHYSICS_3D_ENABLED
const float PhysicsServer::world_scale_3d = 1.0f;
#endif

PhysicsServer::PhysicsServer() {
    ASSERT(singleton == NULL);
    singleton = this;
    
    #if PHYSICS_2D_ENABLED
    setGravity(Vector2(0.0f, 100.0f));
    #endif

    #if PHYSICS_3D_ENABLED
    dInitODE2(0);
    world_3d = dWorldCreate();
    // dWorldSetCFM(world_3d, 0.000001);
    // dWorldSetERP(world_3d, 0.000001);
    // dWorldSetQuickStepNumItlerations(world_3d, 100);
    main_space = dHashSpaceCreate(NULL);
    main_group = dJointGroupCreate(0);
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

void callNearCallback(void *data, dGeomID o1, dGeomID o2) {
    PhysicsServer::getSingleton()->nearCallback(data, o1, o2);
}

void PhysicsServer::nearCallback(void *data, dGeomID o1, dGeomID o2) {

    CollisionShape3D* A = reinterpret_cast<CollisionShape3D*>(dGeomGetData(o1));
    CollisionShape3D* B = reinterpret_cast<CollisionShape3D*>(dGeomGetData(o2));

    ASSERT(A);
    ASSERT(B);

    if (A->getAttachedBody() == B->getAttachedBody()) {
        return;
    }

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    // if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
    //     return;

    dContact* contacts = (dContact*)malloc(MAX_CONTACTS * sizeof(dContact));
    contact_pool.push_back(contacts);

    int i;
    for (i = 0; i < MAX_CONTACTS; i++) {
        contacts[i].surface.mode = dContactMu2;
        contacts[i].surface.mu = dInfinity;
        contacts[i].surface.mu2 = dInfinity;

        // contacts[i].surface.mode = dContactSoftERP;
        // contacts[i].surface.mu = 0.0;
        // contacts[i].surface.soft_cfm = 0.000001;
        // contacts[i].surface.soft_erp = 0.000001;
    }

    int contact_count = dCollide(o1, o2, MAX_CONTACTS | CONTACTS_UNIMPORTANT, &contacts[0].geom, sizeof(dContact));
    
    for (i = 0; i < contact_count; i++) {
        dJointID c = dJointCreateContact(world_3d, main_group, &contacts[i]);
        dJointAttach(c, b1, b2);

        CollisionShape3D::Contact* contact = new CollisionShape3D::Contact(&contacts[i], A, B);
        A->contacts.push_back(contact);
        B->contacts.push_back(contact);
    }
}

void PhysicsServer::physicsProcess(float delta) {

    if (delta <= 0.0f) {
        return;
    }

    #if PHYSICS_2D_ENABLED
    world_2d.Step(time_step * delta, velocity_iterations, position_iterations);
    #endif

    #if PHYSICS_3D_ENABLED
    for (dContact* contacts : contact_pool) {
        free(contacts);
    }
    contact_pool.clear();

    dSpaceCollide(main_space, 0, &callNearCallback);
    dWorldQuickStep(world_3d, time_step * delta);
    dJointGroupEmpty(main_group);
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
    gravity_3d.set(value.x, value.y, value.z);
    dWorldSetGravity(world_3d, value.x, value.y, value.z);
}
Vector3 PhysicsServer::getGravity3() {
    return gravity_3d;
}

dBodyID PhysicsServer::createBody3() {
    return dBodyCreate(world_3d);
}

void PhysicsServer::destroyBody3(dBodyID body) {
    return dBodyDestroy(body);
}

#endif

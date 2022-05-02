#include "physics_server.h"

#include <box2d/box2d.h>
#include <stdlib.h>

#include "common/utils.h"
#include "common/draw.h"

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

#define MAX_CONTACTS 8

void callNearCallback(void *data, dGeomID o1, dGeomID o2) {
    PhysicsServer::getSingleton()->nearCallback(data, o1, o2);
}

void PhysicsServer::nearCallback(void *data, dGeomID o1, dGeomID o2) {

    // if (string((const char*)dGeomGetData(o2)) != "Bruh") {
    //     OS::print(to_string(OS::getTime()) + " COLLISION");
    //     // OS::print(((Node*)dGeomGetData(o1))->getName());
    //     // OS::print(((Node*)dGeomGetData(o2))->getName());
    // }
    // else {
    //     // OS::print(to_string(OS::getTime()) + " Floor");
    // }

    if (int(dGeomGetData(o1) == NULL) + int(dGeomGetData(o2) == NULL) != 1) {
        return;
    }

    (void)data;
    int i;
    // if (o1->body && o2->body) return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;

    dContact contact[MAX_CONTACTS]; // up to MAX_CONTACTS contacts per body-body
    for (i = 0; i < MAX_CONTACTS; i++) {
        /*contact[i].surface.mode = dContactBounce | dContactSoftCFM | dContactApprox1;*/
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.bounce = 0.0;
        contact[i].surface.bounce_vel = 0.1;
        /*contact[i].surface.soft_cfm = 0.01;*/
    }
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                        sizeof(dContact));
    if (numc) {
        dMatrix3 RI;
        dRSetIdentity(RI);
        for (i = 0; i < numc; i++) {
            dJointID c = dJointCreateContact(world_3d, main_group, contact + i);
            dJointAttach(c, b1, b2);
        }
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
    dBodyID ret = dBodyCreate(world_3d);
    dBodySetMaxAngularSpeed(ret, 0);
    return ret;
}

void PhysicsServer::destroyBody3(dBodyID body) {
    return dBodyDestroy(body);
}

#endif

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
    world_3d = phys_common.createPhysicsWorld();
    
    world_3d->setIsDebugRenderingEnabled(true);
    react::DebugRenderer& debugRenderer = world_3d->getDebugRenderer(); 
    debugRenderer.setIsDebugItemDisplayed(react::DebugRenderer::DebugItem::COLLISION_SHAPE, true); 

    setGravity(Vector3(0.0f, 100.0f, 0.0f));
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

void PhysicsServer::physicsProcess(float delta) {

    if (delta <= 0.0f) {
        return;
    }

    #if PHYSICS_2D_ENABLED
    world_2d.Step(time_step * delta, velocity_iterations, position_iterations);
    #endif
    #if PHYSICS_3D_ENABLED
    world_3d->update(time_step * delta);
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
    world_3d->setGravity(gravity_3d);
}
Vector3 PhysicsServer::getGravity3() {
    return gravity_3d;
}

react::CollisionBody* PhysicsServer::createBody3(react::Transform transform, PhysicsBody3D::TYPE type) {
    switch (type) {
        case PhysicsBody3D::TYPE::STATIC: return world_3d->createCollisionBody(transform);
        case PhysicsBody3D::TYPE::RIGID: return world_3d->createRigidBody(transform);
        #if DEBUG_ENABLED
        default: ASSERT(false);
        #endif
    }
}

void PhysicsServer::destroyBody3(react::CollisionBody* body) {
    if (react::RigidBody* rigid = dynamic_cast<react::RigidBody*>(body)) {
        world_3d->destroyRigidBody(rigid);
    }
    else {
        world_3d->destroyCollisionBody(body);
    }
}
#endif

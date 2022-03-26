#include "physics_server.h"

#include <raylib-cpp.hpp>
#include <box2d/box2d.h>
#include <stdlib.h>

#include "engine/src/utils.h"

PhysicsServer* PhysicsServer::singleton = NULL;

PhysicsServer::PhysicsServer() {
    assert(singleton == NULL);
    singleton = this;
    
    gravity = b2Vec2(0.0F, -10.0F);
    world = b2World(gravity);

    time_step = 1.0f / 60.0f;
    velocity_iterations = 8;
    position_iterations = 3;
}

PhysicsServer* PhysicsServer::getSingleton() {
    assert(singleton);
    return singleton;
}

void PhysicsServer::physicsProcess(float delta) {
    world.Step(time_step, velocity_iterations, position_iterations);
}

void PhysicsServer::setGravity(Vector2 value) {
    gravity.Set(value.x, value.y * -1);
    world.SetGravity(gravity);
}
void PhysicsServer::setGravity(b2Vec2 value) {
    gravity.Set(value.x, value.y * -1);
    world.SetGravity(gravity);
}
Vector2 PhysicsServer::getGravity() {
    Vector2 ret;
    ret.x = gravity.x;
    ret.y = gravity.y * -1;
    return ret;
}

b2Body* PhysicsServer::createBody(b2BodyType type) {
    b2BodyDef definition;
    definition.type = type;
    b2Body* body = world.CreateBody(&definition);
    return body;
}

b2Body* PhysicsServer::createBody(float x_pos, float y_pos, b2BodyType type) {
    b2BodyDef definition;
    definition.type = type;
    definition.position.Set(x_pos, y_pos);
    b2Body* body = world.CreateBody(&definition);
    return body;
}
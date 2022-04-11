#include "physics_server.h"

#include "engine/src/raylib_include.h"
#include <box2d/box2d.h>
#include <stdlib.h>

#include "common/utils.h"
#include "common/draw.h"

PhysicsServer* PhysicsServer::singleton = NULL;
const float PhysicsServer::world_scale = 12.0f;

class FooDraw : public b2Draw {
    public:
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {}
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
            for (int i = 0; i < vertexCount; i += 1) {
                Vector2 start_vertex = PhysicsServer::phys2World(vertices[i]);
                Vector2 end_vertex = PhysicsServer::phys2World(vertices[i + 1 == vertexCount ? 0 : i + 1]);
                Draw::drawLine(start_vertex.x, start_vertex.y, end_vertex.x, end_vertex.y, GREEN);
            }
        }
        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {}
        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {}
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {}
        void DrawTransform(const b2Transform& xf) {}
        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) {}
};

PhysicsServer::PhysicsServer() {
    assert(singleton == NULL);
    singleton = this;
    
    setGravity(b2Vec2(0.0F, 100.0F));
    world.SetGravity(gravity);

    FooDraw* instance = new FooDraw;
    world.SetDebugDraw(instance);
    instance->SetFlags(b2Draw::e_shapeBit);

    time_step = 1.0f;
    velocity_iterations = 8;
    position_iterations = 3;
}

b2Vec2 PhysicsServer::world2Phys(Vector2 world_position) {
    return b2Vec2(world2Phys(world_position.x), world2Phys(world_position.y));
}
float PhysicsServer::world2Phys(float world_position) {
    return world_position / world_scale;
}
Vector2 PhysicsServer::phys2World(b2Vec2 physics_position) {
    return Vector2(phys2World(physics_position.x), phys2World(physics_position.y));
}
float PhysicsServer::phys2World(float physics_position) {
    return physics_position * world_scale;
}

PhysicsServer* PhysicsServer::getSingleton() {
    assert(singleton);
    return singleton;
}

void PhysicsServer::physicsProcess(float delta) {
    world.Step(time_step * delta, velocity_iterations, position_iterations);
    world.DebugDraw();
}

void PhysicsServer::setGravity(Vector2 value) {
    gravity.Set(value.x, value.y);
    world.SetGravity(gravity);
}
void PhysicsServer::setGravity(b2Vec2 value) {
    gravity.Set(value.x, value.y);
    world.SetGravity(gravity);
}
Vector2 PhysicsServer::getGravity() {
    Vector2 ret;
    ret.x = gravity.x;
    ret.y = gravity.y * -1;
    return ret;
}

b2Body* PhysicsServer::createBody(const b2BodyDef* definition) {
    return world.CreateBody(definition);
}

void PhysicsServer::destroyBody(b2Body* body) {
    world.DestroyBody(body);
}
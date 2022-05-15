#include "engine/compiler_settings.h"

#if PHYSICS_2D_ENABLED

#include "collision_object_2d.h"

#include "common/draw.h"
#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/physics_body.h"

void CollisionObject2D::ready() {
    super::ready();
}

void CollisionObject2D::draw() {
    super::draw();

    // return;

    if (!isGlobalVisible() || collision_object == NULL) {
        return;
    }

    switch (getType()) {
        case b2Shape::e_polygon: {
            
            b2Body* body = getAttachedFixture()->GetBody();
            Vector2 body_position = PhysicsServer::phys2World2(body->GetPosition());
            shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(collision_object);

            Vector2 scale = Vector2(1, 1);

            // if (Node2D* parent_2d = dynamic_cast<Node2D*>(attached_body->getParent())) {
            //     scale = parent_2d->getGlobalScale();
            // }

            // scale = getGlobalScale();

            for (int i = 0; i < shape->m_count; i += 1) {
                Vector2 start_vertex = (PhysicsServer::phys2World2(shape->m_vertices[i]) * scale).rotated(body->GetAngle()) + body_position;
                Vector2 end_vertex = (PhysicsServer::phys2World2(shape->m_vertices[i + 1 == shape->m_count ? 0 : i + 1]) * scale).rotated(body->GetAngle()) + body_position;
                Draw::drawLine(start_vertex, end_vertex, Colour::RED());
            }
        }
    }
}

void CollisionObject2D::onParentGlobalScaleChanged(Vector2 old_global_scale) {
    super::onParentGlobalScaleChanged(old_global_scale);

    if (!getUseRelativeScale() || attached_fixture == NULL) {
        return;
    }

    scaleChanged(old_global_scale);
}

void CollisionObject2D::enteredTree() {
    super::enteredTree();
    scaleChanged(base_scale);
}

void CollisionObject2D::scaleChanged(Vector2 old_scale) {

    if (attached_fixture == NULL) {
        return;
    }

    base_scale = getGlobalScale();
    Vector2 multiplier = base_scale / (old_scale * getScale());

    switch (attached_fixture->GetShape()->GetType()) {
        case b2Shape::Type::e_polygon: {

            shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(collision_object);
            for (int i = 0; i < shape->m_count; i++) {
                shape->m_vertices[i].x *= multiplier.x; 
                shape->m_vertices[i].y *= multiplier.y; 
            }

            SIGNAL_SHAPE_CHANGED.emit();

        }
    }
}

void CollisionObject2D::setBoxShape(float width, float height, Vector2 position, float rotation) {
    base_scale = getGlobalScale();

    width = PhysicsServer::world2Phys2(width) * base_scale.x;
    height = PhysicsServer::world2Phys2(height) * base_scale.y;

    shared_ptr<b2PolygonShape> shape = make_shared<b2PolygonShape>();
    shape->SetAsBox(width / 2.0f, height / 2.0f, b2Vec2((width / 2.0f) + PhysicsServer::world2Phys2(position.x), (height / 2.0f) + PhysicsServer::world2Phys2(position.y)), rotation);

    collision_object = shape;
    
    SIGNAL_SHAPE_CHANGED.emit();
}

void CollisionObject2D::setBoxShape(Vector2 size, Vector2 position, float rotation) {
    setBoxShape(size.x, size.y, position, rotation);
}

bool CollisionObject2D::hasShape() {
    return collision_object != NULL;
}

shared_ptr<b2Shape> CollisionObject2D::getShape() {
    ASSERT(collision_object != NULL);
    return collision_object;
}

b2Shape::Type CollisionObject2D::getType() {
    ASSERT(collision_object != NULL);
    return collision_object->GetType();
}

void CollisionObject2D::attachToBody(PhysicsBody2D* body, b2Fixture* fixture) {
    ASSERT(attached_body == NULL && attached_fixture == NULL);
    attached_body = body;
    attached_fixture = fixture;
}

void CollisionObject2D::detachFromBody() {
    ASSERT(attached_body != NULL && attached_fixture != NULL);
    attached_body = NULL;
    attached_fixture = NULL;
}

#endif
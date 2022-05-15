#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "collision_object_3d.h"

#include "common/draw.h"
#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/physics_body_3d.h"

void CollisionObject3D::ready() {
    super::ready();
}

void CollisionObject3D::draw() {
    super::draw();

    if (!isGlobalVisible() || physics_shape == NULL) {
        return;
    }

    // switch (getType()) {
    //     case b2Shape::e_polygon: {
            
    //         b2Body* body = getAttachedFixture()->GetBody();
    //         Vector2 body_position = PhysicsServer::phys2World2(body->GetPosition());
    //         shared_ptr<b2PolygonShape> physics_shape = reinterpret_pointer_cast<b2PolygonShape>(physics_shape);

    //         Vector2 scale = Vector2(1, 1);

    //         // if (Node3D* parent_3d = dynamic_cast<Node3D*>(attached_body->getParent())) {
    //         //     scale = parent_3d->getGlobalScale();
    //         // }

    //         // scale = getGlobalScale();

    //         for (int i = 0; i < physics_shape->m_count; i += 1) {
    //             Vector2 start_vertex = (PhysicsServer::phys2World2(physics_shape->m_vertices[i]) * scale).rotated(body->GetAngle()) + body_position;
    //             Vector2 end_vertex = (PhysicsServer::phys2World2(physics_shape->m_vertices[i + 1 == physics_shape->m_count ? 0 : i + 1]) * scale).rotated(body->GetAngle()) + body_position;
    //             Draw::drawLine(start_vertex, end_vertex, Colour::RED());
    //         }
    //     }
    // }
}

void CollisionObject3D::setPosition(Vector3 value) {
    super::setPosition(value);

    if (attached_body) {
        btCompoundShape& container = attached_body->getShapeContainer();

        for (int i = 0; i < container.getNumChildShapes(); i++) {
            if (container.getChildShape(i) == physics_shape) {
                btTransform& transform = container.getChildTransform(i);
                transform.setOrigin(PhysicsServer::world2Phys3(getPosition()));
                container.updateChildTransform(i, transform, false);
                break;
            }
        }
    }
}

void CollisionObject3D::setRotation(Quaternion value) {
    super::setRotation(value);

    if (attached_body) {
        btCompoundShape& container = attached_body->getShapeContainer();

        for (int i = 0; i < container.getNumChildShapes(); i++) {
            if (container.getChildShape(i) == physics_shape) {
                btTransform& transform = container.getChildTransform(i);
                transform.setRotation(getRotation());
                container.updateChildTransform(i, transform, true);
                break;
            }
        }
    }
}

void CollisionObject3D::onParentGlobalScaleChanged(Vector3 old_global_scale) {
    super::onParentGlobalScaleChanged(old_global_scale);

    if (!getUseRelativeScale()) {
        return;
    }

    scaleChanged(old_global_scale);
}

void CollisionObject3D::enteredTree() {
    super::enteredTree();
    scaleChanged(base_scale);
}

void CollisionObject3D::scaleChanged(Vector3 old_scale) {

    if (physics_shape == NULL) {
        return;
    }

    base_scale = getGlobalScale();
    Vector3 multiplier = base_scale / (old_scale * getScale());

    // TODO
    // switch (getType()) {
        
    // }
}

void CollisionObject3D::freePhysicsShape() {
    if (physics_shape == NULL) {
        return;
    }

    detachFromBody();
    delete physics_shape;
    physics_shape = NULL;
}

bool CollisionObject3D::hasPhysicsShape() {
    return physics_shape != NULL;
}

btCollisionShape* CollisionObject3D::getPhysicsShape() {
    ASSERT(physics_shape != NULL);
    return physics_shape;
}

void CollisionObject3D::attachToBody(PhysicsBody3D* body) {
    if (attached_body != NULL)
        return;
    attached_body = body;
    attached_body->getShapeContainer().addChildShape(btTransform(getRotation(), PhysicsServer::world2Phys3(getPosition())), physics_shape);
}

void CollisionObject3D::detachFromBody() {
    ASSERT(attached_body != NULL && physics_shape != NULL);
    attached_body->getShapeContainer().removeChildShape(physics_shape);
    attached_body = NULL;
}

void CollisionObject3D::setShape(shared_ptr<Shape3D> value) {
    if (shape != NULL) {
        attached_body->getShapeContainer().removeChildShape(physics_shape);
    }
    
    shape = value;
    shape->applyToCollisionObject(this);
    shape->SIGNAL_CHANGED.connect(&SIGNAL_SHAPE_CHANGED, &Signal<>::emit);

    if (attached_body)
        attached_body->getShapeContainer().addChildShape(btTransform(getRotation(), PhysicsServer::world2Phys3(getPosition())), physics_shape);

    SIGNAL_SHAPE_CHANGED.emit();
}

shared_ptr<Shape3D> CollisionObject3D::getShape() {
    return shape;
}

#endif
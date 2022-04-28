#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "physics_body_3d.h"

#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/collision_shape_3d.h"
#include "engine/src/node/types/timer.h"

#define FLOOR_ANGLE_THRESHOLD 0.01
#define p_floor_max_angle DEG2RAD(45.0f)
#define RIGID_BODY dynamic_cast<react::RigidBody*>(body)

void PhysicsBody3D::physicsProcess(float delta) {
    super::physicsProcess(delta);

    on_floor = false;
    on_wall = false;
    on_ceiling = false;

    if (type != TYPE::RIGID || RIGID_BODY->isSleeping()) {
        return;
    }

    updating_position = true;

    setGlobalPosition(PhysicsServer::phys2World3(Vector3(body->getTransform().getPosition())));
    setGlobalRotation(body->getTransform().getOrientation());
    updating_position = false;

    vector<PhysicsBody3D*> collisions;

    string collisions_text = "Colliding with: ";


    // Vector3 linear_velocity = body->GetLinearVelocity();// + getFinalGravity();
    // b2WorldManifold manifold;

    // for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {

    //     ce->contact->GetWorldManifold(&manifold);
            
    //     if (manifold.separations[0] > 0.0f || manifold.separations[0] > 0.0f) {
    //         continue;
    //     }

    //     PhysicsBody3D* other = reinterpret_cast<PhysicsBody3D*>(ce->other->GetUserData().pointer);
    //     collisions.push_back(other);

    //     bool was_colliding = false;
    //     for (auto i = previous_collisions.begin(); i != previous_collisions.end(); ++i) {
    //         if (*i == other) {
    //             was_colliding = true;
    //             previous_collisions.erase(i);
    //             --i;
    //             break;
    //         }
    //     }
        
    //     if (!was_colliding) {
    //         SIGNAL_COLLIDED.emit(other);
    //     }

    //     SIGNAL_COLLIDING.emit(other);

    //     collisions_text += other->operator string() + " | ";

    //     if (body->GetType() == b2_staticBody) {
    //         continue;
    //     }

    //     if (up_direction.isZero()) {
    //         on_wall = true;
    //     } 
    //     else {

    //         Vector2 normal = manifold.normal;

    //         if (linear_velocity.y > 0.0f && acos(normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { //floor

    //             on_floor = true;
    //             // floor_normal = collision.normal;
    //             // on_floor_body = collision.collider_rid;
    //             // floor_velocity = collision.collider_vel;

    //             // if (p_stop_on_slope) {
    //             //     if ((body_velocity_normal + up_direction).length() < 0.01 && collision.travel.length() < 1) {
    //             //         Transform3D gt = get_global_transform();
    //             //         gt.elements[2] -= collision.travel.slide(up_direction);
    //             //         set_global_transform(gt);
    //             //         return Vector2();
    //             //     }
    //             // }
            
    //         } 
    //         else if (linear_velocity.y < 0.0f && acos(normal.dot(-up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) {
    //             on_ceiling = true;
    //         } 
    //         else if (linear_velocity.x != 0.0f) {
    //             on_wall = true;
    //         }
    //     }    
    // }

    // for (PhysicsBody3D* body : previous_collisions) {
    //     SIGNAL_COLLISION_ENDED.emit(body);
    // }
    // previous_collisions = collisions;

    // addGizmoText("Collisions: " + to_string(collisions.size()), true);
    // addGizmoText(collisions_text, true);
    // addGizmoText("On floor: " + to_string(on_floor), true);
    // addGizmoText("On wall: " + to_string(on_wall), true);
    // addGizmoText("On ceiling: " + to_string(on_ceiling), true);
    // addGizmoText("Velocity: " + PhysicsServer::phys2World2(linear_velocity).toString(), true);

    // updating_position = false;
}

void PhysicsBody3D::setPosition(Vector3 value) {
    super::setPosition(value);

    if (updating_position) {
        return;
    }
    
    if (body) {
        react::Transform transform = body->getTransform();
        transform.setPosition(PhysicsServer::world2Phys3(getGlobalPosition()));
        body->setTransform(transform);
    }
}

void PhysicsBody3D::setRotation(Vector3 value) {
    super::setRotation(value);

    if (updating_position) {
        return;
    }
    
    if (body) {
        react::Transform transform = body->getTransform();
        transform.setOrientation(react::Quaternion::fromEulerAngles(getGlobalRotation()));
        body->setTransform(transform);
    }
}

bool PhysicsBody3D::isOnFloor() {
    return on_floor;
}

bool PhysicsBody3D::isOnWall() {
    return on_wall;
}

bool PhysicsBody3D::isOnCeiling() {
    return on_ceiling;
}

void PhysicsBody3D::setLinearVelocity(Vector3 value) {
    ASSERT(getType() == TYPE::RIGID);
    RIGID_BODY->setLinearVelocity(PhysicsServer::world2Phys3(value));
}

Vector3 PhysicsBody3D::getLinearVelocity() {
    ASSERT(getType() == TYPE::RIGID);
    return PhysicsServer::phys2World3(RIGID_BODY->getLinearVelocity());
}

void PhysicsBody3D::setFixedRotation(bool value) {
    ASSERT(getType() == TYPE::RIGID);
    RIGID_BODY->setAngularLockAxisFactor(value ? Vector3::ZERO() : Vector3::ONE());
}

bool PhysicsBody3D::isFixedRotation() {
    ASSERT(getType() == TYPE::RIGID);
    return ((Vector3)RIGID_BODY->getAngularLockAxisFactor()).isZero();
}

void PhysicsBody3D::setApplyGravity(bool value) {
    ASSERT(getType() == TYPE::RIGID);
    apply_gravity = value;
    RIGID_BODY->enableGravity(apply_gravity);
}

bool PhysicsBody3D::isApplyGravity() {
    return apply_gravity;
}

Vector3 PhysicsBody3D::getFinalGravity() {
    if (!apply_gravity) {
        return Vector3::ZERO();
    }
    return PhysicsServer::getSingleton()->getGravity3();
}

void PhysicsBody3D::setUpDirection(Vector3 value) {
    up_direction = value;
}

Vector3 PhysicsBody3D::getUpDirection() {
    return up_direction;
}

void PhysicsBody3D::enteredTree() {
    createBody();
    super::enteredTree();
}

void PhysicsBody3D::removedFromNode(Node* former_parent_node) {
    super::removedFromNode(former_parent_node);
    destroyBody();
}

void PhysicsBody3D::addChild(Node* child) {
    if (CollisionShape3D* collision_shape = dynamic_cast<CollisionShape3D*>(child)) {
        addShape(collision_shape);
    }
    super::addChild(child);
}

void PhysicsBody3D::removeChild(Node* child) {
    super::removeChild(child);
    if (CollisionShape3D* collision_shape = dynamic_cast<CollisionShape3D*>(child)) {
        removeShape(collision_shape);
    }
}

void PhysicsBody3D::addShape(CollisionShape3D* shape) {

    ASSERT(!vectorContainsValue(added_shapes, shape));
    ASSERT(!shape->isAttachedToBody());
    
    if (body != NULL && shape->hasShape()) {
        createShapeCollider(shape);
    }

    shape->SIGNAL_POLYGON_CHANGED.connect(&PhysicsBody3D::onShapePolygonChanged, this, false, shape);

    added_shapes.push_back(shape);
    SIGNAL_SHAPE_ADDED.emit(shape);
}

void PhysicsBody3D::removeShape(CollisionShape3D* shape) {
    SIGNAL_SHAPE_REMOVED.emit(shape);

    ASSERT(vectorContainsValue(added_shapes, shape));
    ASSERT(shape->isAttachedToBody());

    if (isInsideTree()) {
        destroyShapeCollider(shape);
    }

    vectorRemoveValue(&added_shapes, shape);
    SIGNAL_SHAPE_REMOVED.emit(shape);
}

void PhysicsBody3D::setType(TYPE value) {
    if (type == value) {
        return;
    }

    type = value;
    if (body != NULL) {
        destroyBody();
        createBody();
    }
}

PhysicsBody3D::TYPE PhysicsBody3D::getType() {
    return type;
}

void PhysicsBody3D::createBody() {
    ASSERT(body == NULL);

    body = PhysicsServer::getSingleton()->createBody3(react::Transform{getGlobalPosition(), react::Quaternion::fromEulerAngles(getGlobalRotation())}, type);
    for (CollisionShape3D* shape : added_shapes) {
        createShapeCollider(shape);
    }
}

void PhysicsBody3D::destroyBody() {
    PhysicsServer::getSingleton()->destroyBody3(body);
    body = NULL;
}


void PhysicsBody3D::onShapePolygonChanged(CollisionShape3D* shape) {

    if (body == NULL) {
        return;
    }

    if (shape->isAttachedToBody() && shape->getCollider() != NULL) {
        destroyShapeCollider(shape);
    }

    if (shape->hasShape()) {
        createShapeCollider(shape);
    }
}

void PhysicsBody3D::createShapeCollider(CollisionShape3D* shape) {
    ASSERT(body != NULL);
    ASSERT(shape->hasShape());

    shape->attachToBody(this, body->addCollider(shape->getShape(), react::Transform::identity()));
}

void PhysicsBody3D::destroyShapeCollider(CollisionShape3D* shape) {
    ASSERT(body != NULL);
    body->removeCollider(shape->getCollider());
    shape->SIGNAL_POLYGON_CHANGED.disconnect(this);
    shape->detachFromBody();
}

void PhysicsBody3D::onParentGlobalPositionChanged(Vector3 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);

    if (body) {
        react::Transform transform = body->getTransform();
        transform.setPosition(PhysicsServer::world2Phys3(getGlobalPosition()));
        body->setTransform(transform);
    }
}

#endif
#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "physics_body_3d.h"

#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/collision_shape_3d.h"
#include "engine/src/node/types/timer.h"

#define FLOOR_ANGLE_THRESHOLD 0.01
#define p_floor_max_angle DEG2RAD(45.0f)

Matrix getTransform(const float pos[3], const float R[12]) {
    Matrix ret;
    ret.m0 = R[0];
    ret.m1 = R[4];
    ret.m2 = R[8];
    ret.m3 = 0;
    ret.m4 = R[1];
    ret.m5 = R[5];
    ret.m6 = R[9];
    ret.m7 = 0;
    ret.m8 = R[2];
    ret.m9 = R[6];
    ret.m10 = R[10];
    ret.m11 = 0;
    ret.m12 = pos[0];
    ret.m13 = pos[1];
    ret.m14 = pos[2];
    ret.m15 = 1;
    return ret;
}

void PhysicsBody3D::physicsProcess(float delta) {
    super::physicsProcess(delta);

    on_floor = false;
    on_wall = false;
    on_ceiling = false;

    // if (isKinematic()) {
    //     return;
    // }

    updating_position = true;

    const btTransform& transform = getBody()->getWorldTransform();
    // setGlobalPosition(PhysicsServer::phys2World3(transform.getOrigin()));

    OS::print(getGlobalRotation().toString());

    setGlobalRotation(transform.getRotation());

    updating_position = false;

    // vector<PhysicsBody3D*> collisions;

    // string collisions_text = "Colliding with: ";

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
    
    if (isInsideTree()) {
        btTransform& transform = body->getWorldTransform();
        transform.setOrigin(PhysicsServer::world2Phys3(getGlobalPosition()));
        body->setWorldTransform(transform);
    }
}

void PhysicsBody3D::setRotation(Quaternion value) {
    super::setRotation(value);

    if (updating_position) {
        return;
    }
    
    if (isInsideTree()) {
        btTransform& transform = body->getWorldTransform();
        transform.setRotation(getRotation());
        body->setWorldTransform(transform);
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
    ASSERT(body);
    body->setLinearVelocity(value);
}

Vector3 PhysicsBody3D::getLinearVelocity() {
    ASSERT(body);
    return body->getLinearVelocity();
}

void PhysicsBody3D::setFixedRotation(bool value) {
    fixed_rotation = value;
    body->setAngularFactor(value ? Vector3::ZERO() : Vector3::ONE());
}

bool PhysicsBody3D::isFixedRotation() {
    return fixed_rotation;
}

void PhysicsBody3D::setGravityScale(Vector3 value) {
    body->setGravity(PhysicsServer::getSingleton()->getGravity3() * value);
}

Vector3 PhysicsBody3D::getGravityScale() {
    return body->getGravity() / PhysicsServer::getSingleton()->getGravity3();
}

Vector3 PhysicsBody3D::getFinalGravity() {
    return body->getGravity();
}

void PhysicsBody3D::enteredTree() {
    PhysicsServer::getWorld3()->addRigidBody(body);
    
    btTransform& transform = body->getWorldTransform();
    transform.setOrigin(PhysicsServer::world2Phys3(getGlobalPosition()));
    transform.setRotation(getGlobalRotation());
    body->setWorldTransform(transform);

    super::enteredTree();
}

void PhysicsBody3D::removedFromNode(Node* former_parent_node) {
    super::removedFromNode(former_parent_node);
    PhysicsServer::getWorld3()->removeRigidBody(body);
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
    
    if (shape->hasShape()) {
        shape->attachToBody(this);
    }

    shape->SIGNAL_POLYGON_CHANGED.connect(this, &PhysicsBody3D::onShapePolygonChanged, false, shape);

    added_shapes.push_back(shape);
    SIGNAL_SHAPE_ADDED.emit(shape);
}

void PhysicsBody3D::removeShape(CollisionShape3D* shape) {
    SIGNAL_SHAPE_REMOVED.emit(shape);

    ASSERT(vectorContainsValue(added_shapes, shape));
    
    if (shape->isAttachedToBody()) {
        shape->detachFromBody();
    }

    vectorRemoveValue(&added_shapes, shape);
    SIGNAL_SHAPE_REMOVED.emit(shape);
}

void PhysicsBody3D::setType(TYPE value) {
    if (type == value) {
        return;
    }
    type = value;
    recreateBody();
}

PhysicsBody3D::TYPE PhysicsBody3D::getType() {
    return type;
}

void PhysicsBody3D::recreateBody() {

    if (body == NULL) {

        btTransform transform;

        if (isInsideTree()) {
            transform.setOrigin(PhysicsServer::world2Phys3(getGlobalPosition()));
            transform.setRotation(getGlobalRotation());
        }
        else {
            transform.setOrigin(PhysicsServer::world2Phys3(getPosition()));
            transform.setRotation(getRotation());
        }

        btDefaultMotionState* state = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo definition = btRigidBody::btRigidBodyConstructionInfo(mass, state, &shape_container);
        body = new btRigidBody(definition);

        if (isInsideTree())
            PhysicsServer::getWorld3()->addCollisionObject(body);
    }
    else {
        
    }

    // setFixedRotation(fixed_rotation);
    // setApplyGravity(apply_gravity);

    // if (kinematic) {
    //     dBodySetKinematic(body);
    // }
    // else {
    //     dBodySetDynamic(body);
    // }

    for (CollisionShape3D* shape : added_shapes) {
        if (shape->hasShape()) {
            shape->attachToBody(this);
        }
    }
}

void PhysicsBody3D::onShapePolygonChanged(CollisionShape3D* shape) {
    if (shape->isAttachedToBody()) {
        shape->detachFromBody();
    }
    shape->attachToBody(this);
}


void PhysicsBody3D::onParentGlobalPositionChanged(Vector3 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);

    if (isInsideTree()) {
        btTransform& transform = body->getWorldTransform();
        transform.setOrigin(PhysicsServer::world2Phys3(getGlobalPosition()));
    }
}

#endif
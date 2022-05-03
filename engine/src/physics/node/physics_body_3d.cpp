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

    // const float* rot = dBodyGetRotation(body);
    const float* pos = dBodyGetPosition(body);

    Matrix mat = MatrixIdentity();
    // mat.m0 = rot[0];
    // mat.m1 = rot[4];
    // mat.m2 = rot[8];
    // mat.m4 = rot[1];
    // mat.m5 = rot[5];
    // mat.m6 = rot[9];
    // mat.m8 = rot[2];
    // mat.m9 = rot[6];
    // mat.m10 = rot[10];
    
    mat.m12 = pos[0];
    mat.m13 = pos[1];
    mat.m14 = pos[2];

    mat.m3 = 0;
    mat.m7 = 0;
    mat.m11 = 0;
    mat.m15 = 1;

    setGlobalPosition(PhysicsServer::phys2World3(Vector3Transform(Vector3::ZERO(), mat)));
    // setRotation(QuaternionFromMatrix(mat));

    // const float* rot = dBodyGetQuaternion(body);
    // Quaternion quat = Quaternion(rot[1], rot[2], rot[3], rot[0]);
    // setRotation(quat);

    // setGlobalPosition(PhysicsServer::phys2World3(Vector3(body_pos[0], body_pos[1], body_pos[2])));


    // Matrix mat = getTransform(dBodyGetPosition(body), dBodyGetRotation(body));
    // setGlobalPosition(Vector3Transform(Vector3::ZERO(), mat));
    // setGlobalRotation(QuaternionToEuler(QuaternionFromMatrix(mat)));

    // mat.m0 = rot[0];
    // mat.m1 = rot[4];
    // mat.m2 = rot[8];
    // mat.m4 = rot[1];
    // mat.m5 = rot[5];
    // mat.m6 = rot[9];
    // mat.m8 = rot[2];
    // mat.m9 = rot[6];
    // mat.m10 = rot[10];

    // Quaternion a = QuaternionFromMatrix(mat);
    // Vector3 b = QuaternionToEuler(a);

    // setGlobalRotation(b);

    // const float* q1 = dBodyGetQuaternion(body);
    // Quaternion q2;
    // q2.x = q1[1];
    // q2.y = q1[2];
    // q2.z = q1[3];
    // q2.w = q1[0];
    // setGlobalRotation(QuaternionToEuler(q2));

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
    
    if (body) {
        setBodyPosition(PhysicsServer::world2Phys3(getGlobalPosition()));
    }
}

void PhysicsBody3D::setRotation(Vector3 value) {
    super::setRotation(value);

    return;

    if (updating_position) {
        return;
    }
    
    if (body) {

        Vector3 v = getGlobalRotation();
        Matrix rot = QuaternionToMatrix(rotation);
        dMatrix3 matrix;

        matrix[0] = rot.m0;
        matrix[4] = rot.m1;
        matrix[8] = rot.m2;
        matrix[1] = rot.m4;
        matrix[5] = rot.m5;
        matrix[9] = rot.m6;
        matrix[2] = rot.m8;
        matrix[6] = rot.m9;
        matrix[10] = rot.m10;

        dBodySetRotation(body, matrix);

        // Vector3 rotation = getGlobalRotation();
        // Quaternion source = QuaternionFromEuler(rotation.z, rotation.y, rotation.x);
        
        // dQuaternion dest;
        // dest[1] = source.x;
        // dest[2] = source.y;
        // dest[3] = source.z;
        // dest[0] = source.w;

        // dBodySetQuaternion(body, dest);
    }
}

void PhysicsBody3D::setRotation(Quaternion value) {
    super::setRotation(value);

    return;

    if (updating_position) {
        return;
    }
    
    if (body) {

        Vector3 v = getGlobalRotation();
        Matrix rot = QuaternionToMatrix(rotation);
        dMatrix3 matrix;

        matrix[0] = rot.m0;
        matrix[4] = rot.m1;
        matrix[8] = rot.m2;
        matrix[1] = rot.m4;
        matrix[5] = rot.m5;
        matrix[9] = rot.m6;
        matrix[2] = rot.m8;
        matrix[6] = rot.m9;
        matrix[10] = rot.m10;

        dBodySetRotation(body, matrix);

        // Vector3 rotation = getGlobalRotation();
        // Quaternion source = QuaternionFromEuler(rotation.z, rotation.y, rotation.x);
        
        // dQuaternion dest;
        // dest[1] = source.x;
        // dest[2] = source.y;
        // dest[3] = source.z;
        // dest[0] = source.w;

        // dBodySetQuaternion(body, dest);
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
    dBodySetLinearVel(body, PhysicsServer::world2Phys3(value.x), PhysicsServer::world2Phys3(value.y), PhysicsServer::world2Phys3(value.z));
}

Vector3 PhysicsBody3D::getLinearVelocity() {
    ASSERT(body);
    
    const float* vel = dBodyGetLinearVel(body);
    return Vector3(vel[0], vel[1], vel[2]);
}

void PhysicsBody3D::setFixedRotation(bool value) {
    ASSERT(!isKinematic());
    dBodySetMaxAngularSpeed(body, value ? 0.0f : dInfinity);
}

bool PhysicsBody3D::isFixedRotation() {
    ASSERT(!isKinematic());
    return dBodyGetMaxAngularSpeed(body) == 0.0f;
}

void PhysicsBody3D::setApplyGravity(bool value) {
    apply_gravity = value;

    if (body) {
        dBodySetGravityMode(body, apply_gravity);
    }
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

void PhysicsBody3D::setKinematic(bool value) {
    if (kinematic == value) {
        return;
    }

    kinematic = value;

    if (body) {
        if (kinematic) {
            dBodySetKinematic(body);
        }
        else {
            dBodySetDynamic(body);
        }
    }
}

bool PhysicsBody3D::isKinematic() {
    return kinematic;
}

void PhysicsBody3D::createBody() {
    ASSERT(body == NULL);

    body = PhysicsServer::getSingleton()->createBody3();
    dBodySetGravityMode(body, apply_gravity);

    Vector3 position = PhysicsServer::world2Phys3(getGlobalPosition());
    dBodySetPosition(body, position.x, position.y, position.z);

    if (kinematic) {
        dBodySetKinematic(body);
    }
    else {
        dBodySetDynamic(body);
    }

    for (CollisionShape3D* shape : added_shapes) {
        if (shape->hasShape()) {
            shape->attachToBody(this);
        }
    }
}

void PhysicsBody3D::destroyBody() {

    for (CollisionShape3D* shape : added_shapes) {
        if (shape->isAttachedToBody()) {
            shape->detachFromBody();
        }
    }

    PhysicsServer::getSingleton()->destroyBody3(body);
    body = NULL;
}

void PhysicsBody3D::setBodyPosition(Vector3 position) {
    if (body == NULL) {
        return;
    }

    dBodySetPosition(body, position.x, position.y, position.z);

    // dGeomID shape = dBodyGetFirstGeom(body);
    // while (shape) {
    //     dGeomSetPosition(shape, position.x, position.y, position.z);
    //     shape = dBodyGetNextGeom(shape);
    // }
}

void PhysicsBody3D::onShapePolygonChanged(CollisionShape3D* shape) {

    if (body == NULL) {
        return;
    }

    if (shape->isAttachedToBody()) {
        shape->detachFromBody();
    }
    shape->attachToBody(this);
}


void PhysicsBody3D::onParentGlobalPositionChanged(Vector3 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);

    if (body) {
        setBodyPosition(PhysicsServer::world2Phys3(getGlobalPosition()));
    }
}

#endif
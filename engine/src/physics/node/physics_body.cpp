#include "physics_body.h"

#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/collision_shape.h"
#include "engine/src/node/types/timer.h"

#define FLOOR_ANGLE_THRESHOLD 0.01
#define p_floor_max_angle DEG2RAD(45.0f)

void PhysicsBody::physicsProcess(float delta) {
    super::physicsProcess(delta);

    updating_position = true;

    setGlobalPosition(PhysicsServer::phys2World(body->GetPosition()));
    setGlobalRotation(body->GetAngle());

    vector<PhysicsBody*> collisions;

    string collisions_text = "Colliding with: ";

    on_floor = false;
    on_wall = false;
    on_ceiling = false;

    Vector2 linear_velocity = body->GetLinearVelocity();// + getFinalGravity();
    b2WorldManifold manifold;

    for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {

        ce->contact->GetWorldManifold(&manifold);
            
        if (manifold.separations[0] > 0.0f || manifold.separations[0] > 0.0f) {
            continue;
        }

        PhysicsBody* other = reinterpret_cast<PhysicsBody*>(ce->other->GetUserData().pointer);
        collisions.push_back(other);

        bool was_colliding = false;
        for (auto i = previous_collisions.begin(); i != previous_collisions.end(); ++i) {
            if (*i == other) {
                was_colliding = true;
                previous_collisions.erase(i);
                --i;
                break;
            }
        }
        
        if (!was_colliding) {
            SIGNAL_COLLIDED.emit(other);
        }

        SIGNAL_COLLIDING.emit(other);

        collisions_text += other->operator string() + " | ";

        if (body->GetType() == b2_staticBody) {
            continue;
        }

        if (up_direction.isZero()) {
            on_wall = true;
        } 
        else {

            Vector2 normal = manifold.normal;

            if (linear_velocity.y > 0.0f && acos(normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { //floor

                on_floor = true;
                // floor_normal = collision.normal;
                // on_floor_body = collision.collider_rid;
                // floor_velocity = collision.collider_vel;

                // if (p_stop_on_slope) {
                //     if ((body_velocity_normal + up_direction).length() < 0.01 && collision.travel.length() < 1) {
                //         Transform2D gt = get_global_transform();
                //         gt.elements[2] -= collision.travel.slide(up_direction);
                //         set_global_transform(gt);
                //         return Vector2();
                //     }
                // }
            
            } 
            else if (linear_velocity.y < 0.0f && acos(normal.dot(-up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) {
                on_ceiling = true;
            } 
            else if (linear_velocity.x != 0.0f) {
                on_wall = true;
            }
        }    
    }

    for (PhysicsBody* body : previous_collisions) {
        SIGNAL_COLLISION_ENDED.emit(body);
    }
    previous_collisions = collisions;

    addGizmoText("Collisions: " + to_string(collisions.size()), true);
    addGizmoText(collisions_text, true);
    addGizmoText("On floor: " + to_string(on_floor), true);
    addGizmoText("On wall: " + to_string(on_wall), true);
    addGizmoText("On ceiling: " + to_string(on_ceiling), true);
    addGizmoText("Velocity: " + PhysicsServer::phys2World(linear_velocity).toString(), true);

    updating_position = false;
}

void PhysicsBody::setPosition(Vector2 value) {
    super::setPosition(value);

    if (updating_position) {
        return;
    }
    
    if (body) {
        b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());
        definition.position.Set(global_position.x, global_position.y);
        body->SetTransform(global_position, body->GetAngle());
    }
}

bool PhysicsBody::isOnFloor() {
    return on_floor;
}

bool PhysicsBody::isOnWall() {
    return on_wall;
}

bool PhysicsBody::isOnCeiling() {
    return on_ceiling;
}

void PhysicsBody::setLinearVelocity(Vector2 value) {
    body->SetLinearVelocity(PhysicsServer::world2Phys(value));
}

Vector2 PhysicsBody::getLinearVelocity() {
    return PhysicsServer::phys2World(body->GetLinearVelocity());
}

void PhysicsBody::setFixedRotation(bool value) {
    body->SetFixedRotation(value);
}

bool PhysicsBody::isFixedRotation() {
    return body->IsFixedRotation();
}

void PhysicsBody::setGravityScale(float value) {
    gravity_scale = value;
    if (apply_gravity) {
        definition.gravityScale = gravity_scale;
        body->SetGravityScale(definition.gravityScale);
    }
}

float PhysicsBody::getGravityScale() {
    return apply_gravity;
}

void PhysicsBody::setApplyGravity(bool value) {
    apply_gravity = value;
    definition.gravityScale = apply_gravity ? gravity_scale : 0.0f;
    body->SetGravityScale(definition.gravityScale);
}

bool PhysicsBody::isApplyGravity() {
    return apply_gravity;
}

Vector2 PhysicsBody::getFinalGravity() {
    if (!apply_gravity) {
        return Vector2::ZERO();
    }
    return PhysicsServer::getSingleton()->getGravity() * gravity_scale;
}

void PhysicsBody::setUpDirection(Vector2 value) {
    up_direction = value;
}

Vector2 PhysicsBody::getUpDirection() {
    return up_direction;
}

void PhysicsBody::enteredTree() {
    b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());
    definition.position.Set(global_position.x, global_position.y);
    createBody();

    super::enteredTree();
}

void PhysicsBody::removedFromNode(Node* former_parent_node) {
    super::removedFromNode(former_parent_node);
    destroyBody();
}

void PhysicsBody::addChild(Node* child) {
    if (CollisionShape* collision_shape = dynamic_cast<CollisionShape*>(child)) {
        addShape(collision_shape);
    }
    super::addChild(child);
}

void PhysicsBody::removeChild(Node* child) {
    super::removeChild(child);
    if (CollisionShape* collision_shape = dynamic_cast<CollisionShape*>(child)) {
        removeShape(collision_shape);
    }
}

void PhysicsBody::addShape(CollisionShape* shape) {

    ASSERT(!vectorContainsValue(added_shapes, shape));
    ASSERT(!shape->isAttachedToBody());
    
    if (body != NULL && shape->hasShape()) {
        createShapeFixture(shape);
    }

    shape->SIGNAL_POLYGON_CHANGED.connect(&PhysicsBody::onShapePolygonChanged, this, false, shape);

    added_shapes.push_back(shape);
    SIGNAL_SHAPE_ADDED.emit(shape);
}

void PhysicsBody::removeShape(CollisionShape* shape) {
    SIGNAL_SHAPE_REMOVED.emit(shape);

    ASSERT(vectorContainsValue(added_shapes, shape));
    ASSERT(shape->isAttachedToBody());

    if (isInsideTree()) {
        destroyShapeFixture(shape);
    }

    vectorRemoveValue(&added_shapes, shape);
    SIGNAL_SHAPE_REMOVED.emit(shape);
}

void PhysicsBody::setType(b2BodyType value) {
    definition.type = value;

    if (body != NULL) {
        body->SetType(value);
    }
}

b2BodyType PhysicsBody::getType() {
    return definition.type;
}

void PhysicsBody::createBody() {
    ASSERT(body == NULL);
    body = PhysicsServer::getSingleton()->createBody(&definition);

    for (CollisionShape* shape : added_shapes) {
        createShapeFixture(shape);
    }
}

void PhysicsBody::destroyBody() {
    PhysicsServer::getSingleton()->destroyBody(body);
    body = NULL;
}


void PhysicsBody::onShapePolygonChanged(CollisionShape* shape) {

    if (body == NULL) {
        return;
    }

    if (shape->isAttachedToBody()) {
        destroyShapeFixture(shape);
    }

    if (shape->hasShape()) {
        createShapeFixture(shape);
    }
}

void PhysicsBody::createShapeFixture(CollisionShape* shape) {
    ASSERT(body != NULL);
    ASSERT(shape->hasShape());

    b2FixtureDef fixture_def;
    fixture_def.shape = shape->getShape().get();
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.1f;

    shape->attachToBody(this, body->CreateFixture(&fixture_def));
}

void PhysicsBody::destroyShapeFixture(CollisionShape* shape) {
    ASSERT(body != NULL);
    body->DestroyFixture(shape->getAttachedFixture());
    shape->SIGNAL_POLYGON_CHANGED.disconnect(this);
    shape->detachFromBody();
}

void PhysicsBody::onParentGlobalPositionChanged(Vector2 old_global_position) {
    super::onParentGlobalPositionChanged(old_global_position);
    b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());

    definition.position.Set(global_position.x, global_position.y);
    if (body != NULL) {
        body->SetTransform(global_position, body->GetAngle());
    }
}
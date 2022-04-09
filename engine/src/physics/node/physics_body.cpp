#include "physics_body.h"

#include "engine/src/raylib_include.h"

#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/collision_shape.h"
#include "engine/src/core/node/node_types/timer.h"

void PhysicsBody::physicsProcess(float delta) {
    Node2D::physicsProcess(delta);

    updating_position = true;

    setGlobalPosition(PhysicsServer::phys2World(body->GetPosition()));
    setGlobalRotation(body->GetAngle());

    vector<PhysicsBody*> collisions;

    int count = 0;
    string collisions_text = "Colliding with: ";
    for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
        // b2Contact* c = ce->contact;
        count++;

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
    }

    for (PhysicsBody* body : previous_collisions) {
        SIGNAL_COLLISION_ENDED.emit(body);
    }
    previous_collisions = collisions;


    addGizmoText("Collisions: " + int2str(count), true);
    addGizmoText(collisions_text, true);

    updating_position = false;
}

void PhysicsBody::setPosition(Vector2 value) {
    Node2D::setPosition(value);

    if (updating_position) {
        return;
    }
    
    b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());
    definition.position.Set(global_position.x, global_position.y);
    if (body != NULL) {
        body->SetTransform(global_position, body->GetAngle());
    }
}

void PhysicsBody::setLinearVelocity(b2Vec2 value) {
    body->SetLinearVelocity(value);
}

void PhysicsBody::setLinearVelocity(Vector2 value) {
    body->SetLinearVelocity(PhysicsServer::world2Phys(value));
}

b2Vec2 PhysicsBody::getLinearVelocity() {
    return body->GetLinearVelocity();
}

void PhysicsBody::addedToNode(Node* parent_node) {
    Node2D::addedToNode(parent_node);

    b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());
    definition.position.Set(global_position.x, global_position.y);
    createBody();
    // getTree()->createTimer(5.0f)->SIGNAL_TIMEOUT.connect<Node>(&PhysicsBody::queueKill, this);
}

void PhysicsBody::removedFromNode(Node* former_parent_node) {
    Node2D::removedFromNode(former_parent_node);
    destroyBody();
}

void PhysicsBody::addChild(Node* child) {
    Node2D::addChild(child);
    if (CollisionShape* collision_shape = dynamic_cast<CollisionShape*>(child)) {
        addShape(collision_shape);
    }
}

void PhysicsBody::removeChild(Node* child) {
    Node2D::removeChild(child);
    if (CollisionShape* collision_shape = dynamic_cast<CollisionShape*>(child)) {
        removeShape(collision_shape);
    }
}

void PhysicsBody::addShape(CollisionShape* shape) {

    assert(!vectorContainsValue(&added_shapes, shape));
    assert(!shape->isAttachedToBody());
    
    if (body != NULL && shape->hasShape()) {
        createShapeFixture(shape);
    }

    shape->SIGNAL_POLYGON_CHANGED.connect(&PhysicsBody::onShapePolygonChanged, this, shape);

    added_shapes.push_back(shape);
    SIGNAL_SHAPE_ADDED.emit(shape);
}

void PhysicsBody::removeShape(CollisionShape* shape) {
    SIGNAL_SHAPE_REMOVED.emit(shape);

    assert(vectorContainsValue(&added_shapes, shape));
    assert(shape->isAttachedToBody());

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
    assert(body == NULL);
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
    assert(body != NULL);

    b2FixtureDef fixture_def;
    fixture_def.shape = shape->getShape();
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.1f;

    shape->attachToBody(this, body->CreateFixture(&fixture_def));
}

void PhysicsBody::destroyShapeFixture(CollisionShape* shape) {
    assert(body != NULL);
    body->DestroyFixture(shape->getAttachedFixture());
    shape->SIGNAL_POLYGON_CHANGED.disconnect(this);
    shape->detachFromBody();
}

void PhysicsBody::onParentGlobalPositionChanged(Vector2 old_global_position) {
    Node2D::onParentGlobalPositionChanged(old_global_position);
    b2Vec2 global_position = PhysicsServer::world2Phys(getGlobalPosition());

    definition.position.Set(global_position.x, global_position.y);
    if (body != NULL) {
        body->SetTransform(global_position, body->GetAngle());
    }
}
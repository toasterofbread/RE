#if PHYSICS_2D_ENABLED

#ifndef INCLUDED_PHYSICS_BODY_2D
#define INCLUDED_PHYSICS_BODY_2D

#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#include "engine/src/node/types/node_2d.h"
#include "engine/src/core/signal.h"

// Forward declarations
class CollisionObject2D;

class PhysicsBody2D: public Node2D {

    public:
    
        REGISTER_NODE(PhysicsBody2D, Node2D, {
            c->template registerProperty<b2BodyType>("type", &NodeType::setType)
            ->template registerProperty<Vector2>("linear_velocity", &NodeType::setLinearVelocity)
            ->template registerProperty<bool>("fixed_rotation", &NodeType::setFixedRotation)
            ->template registerProperty<float>("gravity_scale", &NodeType::setGravityScale)
            ->template registerProperty<bool>("apply_gravity", &NodeType::setApplyGravity)
            ->template registerProperty<Vector2>("up_direction", &NodeType::setUpDirection);
        });

        Signal<CollisionObject2D*> SIGNAL_SHAPE_ADDED;
        Signal<CollisionObject2D*> SIGNAL_SHAPE_REMOVED;

        Signal<PhysicsBody2D*> SIGNAL_COLLIDED; // Emitted on the first frame of collision
        Signal<PhysicsBody2D*> SIGNAL_COLLIDING; // Emitted on every frame of collision (including the first)
        Signal<PhysicsBody2D*> SIGNAL_COLLISION_ENDED; // Emitted on the first frame after collision ends

        void init();
        void physicsProcess(float delta);

        void enteredTree();
        void removedFromNode(Node* former_parent_node);
    
        void addChild(Node* child);
        void removeChild(Node* child);

        void setPosition(Vector2 value);

        bool isOnFloor();
        bool isOnWall();
        bool isOnCeiling();

        // Physics parameters
        void setType(b2BodyType type);
        b2BodyType getType();

        void setLinearVelocity(Vector2 value);
        Vector2 getLinearVelocity();

        void setFixedRotation(bool value);
        bool isFixedRotation();

        void setGravityScale(float value);
        float getGravityScale();

        void setApplyGravity(bool value);
        bool isApplyGravity();

        Vector2 getFinalGravity();

        void setUpDirection(Vector2 value);
        Vector2 getUpDirection();

        b2Body* getBody() { return body; }

    private:
        b2Body* body = NULL;

        void addShape(CollisionObject2D* shape);
        void removeShape(CollisionObject2D* shape);

        void createBody();
        void destroyBody();

        void onShapeShapeChanged(CollisionObject2D* shape);
        void createShapeFixture(CollisionObject2D* shape);
        void destroyShapeFixture(CollisionObject2D* shape);

        void onParentGlobalPositionChanged(Vector2 old_global_position);

        vector<CollisionObject2D*> added_shapes;
        
        b2BodyDef definition;
        bool updating_position = false;
        vector<PhysicsBody2D*> previous_collisions;

        // Runtime data
        bool on_floor = false;
        bool on_wall = false;
        bool on_ceiling = false;

        // Physics parameters
        bool apply_gravity = true;
        float gravity_scale = 1.0f;
        Vector2 up_direction = Vector2(0.0f, -1.0f);
};

#endif
#endif
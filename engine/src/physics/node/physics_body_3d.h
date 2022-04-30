#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#ifndef INCLUDED_PHYSICS_BODY_3D
#define INCLUDED_PHYSICS_BODY_3D

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"

#include <ode/ode.h> 

// Forward declarations
class CollisionShape3D;

class PhysicsBody3D: public Node3D {

    public:
    
        REGISTER_NODE(PhysicsBody3D, Node3D, {
            c->template registerProperty<bool>("type", &NodeType::setKinematic)
            ->template registerProperty<Vector3>("linear_velocity", &NodeType::setLinearVelocity)
            ->template registerProperty<bool>("fixed_rotation", &NodeType::setFixedRotation)
            ->template registerProperty<bool>("apply_gravity", &NodeType::setApplyGravity)
            ->template registerProperty<Vector3>("up_direction", &NodeType::setUpDirection);
        });

        Signal<CollisionShape3D*> SIGNAL_SHAPE_ADDED;
        Signal<CollisionShape3D*> SIGNAL_SHAPE_REMOVED;

        Signal<PhysicsBody3D*> SIGNAL_COLLIDED; // Emitted on the first frame of collision
        Signal<PhysicsBody3D*> SIGNAL_COLLIDING; // Emitted on every frame of collision (including the first)
        Signal<PhysicsBody3D*> SIGNAL_COLLISION_ENDED; // Emitted on the first frame after collision ends

        void physicsProcess(float delta);

        void enteredTree();
        void removedFromNode(Node* former_parent_node);
    
        void addChild(Node* child);
        void removeChild(Node* child);

        void setPosition(Vector3 value);
        void setRotation(Vector3 value);

        bool isOnFloor();
        bool isOnWall();
        bool isOnCeiling();

        // Physics parameters
        void setKinematic(bool value);
        bool isKinematic();

        void setLinearVelocity(Vector3 value);
        Vector3 getLinearVelocity();

        void setFixedRotation(bool value);
        bool isFixedRotation();

        void setApplyGravity(bool value);
        bool isApplyGravity();

        Vector3 getFinalGravity();

        void setUpDirection(Vector3 value);
        Vector3 getUpDirection();

        dBodyID getBody() { return body; }

    private:
        dBodyID body = NULL;

        void addShape(CollisionShape3D* shape);
        void removeShape(CollisionShape3D* shape);

        void createBody();
        void destroyBody();

        void onShapePolygonChanged(CollisionShape3D* shape);
        void createShapeCollider(CollisionShape3D* shape);
        void destroyShapeCollider(CollisionShape3D* shape);

        void onParentGlobalPositionChanged(Vector3 old_global_position);

        vector<CollisionShape3D*> added_shapes;
        
        bool updating_position = false;
        vector<PhysicsBody3D*> previous_collisions;

        // Runtime data
        bool on_floor = false;
        bool on_wall = false;
        bool on_ceiling = false;

        // Physics parameters
        bool apply_gravity = true;
        Vector3 up_direction = Vector3(0.0f, 1.0f, 0.0f);
};

#endif
#endif
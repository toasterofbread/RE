#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#ifndef INCLUDED_PHYSICS_BODY_3D
#define INCLUDED_PHYSICS_BODY_3D

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"

#include <btBulletCollisionCommon.h> 
#include <btBulletDynamicsCommon.h>

// Forward declarations
class CollisionShape3D;

class PhysicsBody3D: public Node3D {

    public:
    
        REGISTER_NODE_WITH_CONSTRUCTOR(PhysicsBody3D, Node3D, {
            c->template registerProperty<Vector3>("linear_velocity", &NodeType::setLinearVelocity)
            ->template registerProperty<bool>("fixed_rotation", &NodeType::setFixedRotation)
            ->template registerProperty<Vector3>("gravity_scale", &NodeType::setGravityScale);
        }, {
            recreateBody();
        });

        Signal<CollisionShape3D*> SIGNAL_SHAPE_ADDED;
        Signal<CollisionShape3D*> SIGNAL_SHAPE_REMOVED;

        Signal<PhysicsBody3D*> SIGNAL_COLLIDED; // Emitted on the first frame of collision
        Signal<PhysicsBody3D*> SIGNAL_COLLIDING; // Emitted on every frame of collision (including the first)
        Signal<PhysicsBody3D*> SIGNAL_COLLISION_ENDED; // Emitted on the first frame after collision ends

        enum class TYPE { RIGID, KINEMATIC, STATIC };

        void physicsProcess(float delta);

        void enteredTree();
        void removedFromNode(Node* former_parent_node);
    
        void addChild(Node* child);
        void removeChild(Node* child);

        void setPosition(Vector3 value);
        void setRotation(Quaternion value);

        bool isOnFloor();
        bool isOnWall();
        bool isOnCeiling();

        // Physics parameters
        void setType(TYPE value);
        TYPE getType();

        void setLinearVelocity(Vector3 value);
        Vector3 getLinearVelocity();

        void setFixedRotation(bool value);
        bool isFixedRotation();

        void setGravityScale(Vector3 value);
        Vector3 getGravityScale();

        void setGravity(Vector3 value);
        Vector3 getGravity();

        Vector3 getFinalGravity();

        btRigidBody* getBody() { return body; }
        btCompoundShape& getShapeContainer() { return shape_container; }

    private:
        TYPE type = TYPE::RIGID;
        btRigidBody* body = NULL;
        btCompoundShape shape_container;
        // btSphereShape shape_container = btSphereShape(1.0f);

        void addShape(CollisionShape3D* shape);
        void removeShape(CollisionShape3D* shape);

        void recreateBody();

        void onShapePolygonChanged(CollisionShape3D* shape);

        void onParentGlobalPositionChanged(Vector3 old_global_position);

        vector<CollisionShape3D*> added_shapes;
        
        bool updating_position = false;
        vector<PhysicsBody3D*> previous_collisions;

        // Runtime data
        bool on_floor = false;
        bool on_wall = false;
        bool on_ceiling = false;

        // Physics parameters
        float mass = 1.0f;
        bool fixed_rotation = false;
};

#endif
#endif
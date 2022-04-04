#ifndef INCLUDED_PHYSICS_BODY
#define INCLUDED_PHYSICS_BODY

#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#include "engine/src/core/node/node_types/node_2d.h"
#include "engine/src/core/signal.h"

// Forward declarations
class CollisionShape;

class PhysicsBody: public Node2D {

    public:
        REGISTER_NODE(PhysicsBody);

        Signal<CollisionShape*> SIGNAL_SHAPE_ADDED;
        Signal<CollisionShape*> SIGNAL_SHAPE_REMOVED;

        void physicsProcess(float delta);

        void addedToNode(Node* parent_node);
        void removedFromNode(Node* former_parent_node);
    
        void addChild(Node* child);
        void removeChild(Node* child);

        void setPosition(Vector2 value);

        // Physics parameters
        void setType(b2BodyType type);
        b2BodyType getType();

        void setLinearVelocity(b2Vec2 value);
        void setLinearVelocity(Vector2 value);
        b2Vec2 getLinearVelocity();

        b2Body* body = NULL;
    private:
        void addShape(CollisionShape* shape);
        void removeShape(CollisionShape* shape);

        void createBody();
        void destroyBody();

        void onShapePolygonChanged(CollisionShape* shape);
        void createShapeFixture(CollisionShape* shape);
        void destroyShapeFixture(CollisionShape* shape);

        void onParentGlobalPositionChanged(Vector2 old_global_position);

        vector<CollisionShape*> added_shapes;
        
        b2BodyDef definition;
        bool updating_position = false;
};

#endif
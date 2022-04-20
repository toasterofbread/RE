#include <box2d/box2d.h>

#include "engine/src/node/types/node_2d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"

// Forward declarations
class PhysicsBody;

class CollisionShape: public Node2D {

    public:
    
        REGISTER_NODE(CollisionShape, Node2D, ({
            c->template registerMethod<Vector2, Vector2, float>("setBoxShape", &NodeType::setBoxShape);
        }));
        
        Signal<> SIGNAL_POLYGON_CHANGED;

        void ready();
        void draw();
    
        bool hasShape();
        shared_ptr<b2Shape> getShape();

        b2Shape::Type getType();

        void setBoxShape(float width, float height, Vector2 position = Vector2{0, 0}, float rotation = 0.0f);
        void setBoxShape(Vector2 size, Vector2 position = Vector2{0, 0}, float rotation = 0.0f);

    protected:

        void onParentGlobalScaleChanged(Vector2 old_global_scale);
        void enteredTree();

    private:

        friend class PhysicsBody;
        
        bool isAttachedToBody() { return attached_body != NULL; }
        void attachToBody(PhysicsBody* body, b2Fixture* fixture);
        void detachFromBody();

        PhysicsBody* getAttachedBody() { return attached_body; }
        PhysicsBody* attached_body = NULL;

        b2Fixture* getAttachedFixture() { return attached_fixture; }
        b2Fixture* attached_fixture = NULL;

        void onScaleChanged(Vector2 old_scale);

        shared_ptr<b2Shape> collision_shape = NULL;
        Vector2 base_scale = Vector2::ONE();
        bool disabled = false; // !todo

        void scaleChanged(Vector2 old_scale);
};

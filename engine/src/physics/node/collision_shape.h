#include <box2d/box2d.h>

#include "engine/src/core/node/node_types/node_2d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"

// Forward declarations
class PhysicsBody;

class CollisionShape: public Node2D {

    public:
    
        REGISTER_NODE(CollisionShape, Node2D);
        
        Signal<> SIGNAL_POLYGON_CHANGED;

        void ready();
        void draw();
    
        bool hasShape();
        b2Shape* getShape();

        b2Shape::Type getType();

        void setBoxShape(float width, float height, Vector2 position = Vector2{0, 0}, float rotation = 0.0f);
        void setBoxShape(Vector2 size, Vector2 position = Vector2{0, 0}, float rotation = 0.0f);

    protected:

        void onParentGlobalScaleChanged(Vector2 old_global_scale);

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

        b2Shape* collision_shape = NULL;
        bool disabled = false; // !todo
};

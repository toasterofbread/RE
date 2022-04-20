#ifndef INCLUDED_CAMERA3D
#define INCLUDED_CAMERA3D

#include "node/types/node_3d.h"
#include "core/signal.h"

// Forward declarations
class SceneTree;

#ifdef Camera3D
#undef Camera3D
#endif

class Camera3D: public Node3D {
    
    public:
        REGISTER_NODE(Camera3D, Node3D, {});

        void kill();

        bool isEnabled();
        void enable();
        void disable();

        void setZoom(Vector2 value);
        Vector2 getZoom();

        Vector2 getDrawOffset();
        Vector2 getDrawZoom();
        float getDrawRotation();

        Vector2 getCenter();
    
    protected:
        void removedFromNode(Node* former_parent_node);
    
    private:
        Vector2 zoom = Vector2::ONE();
};

#endif
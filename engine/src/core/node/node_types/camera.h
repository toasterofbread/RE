#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA

#include "core/node/node_types/node_2d.h"
#include "core/signal.h"

// Forward declarations
class SceneTree;

class Camera: public Node2D {
    
    public:
        REGISTER_NODE(Camera, Node2D);

        void kill();

        bool isEnabled();
        void enable();
        void disable();

        Vector2 getGlobalOffset();
    
    protected:
        void removedFromNode(Node* former_parent_node);
};

#endif
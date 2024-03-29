#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA

#include "node/types/node_2d.h"
#include "core/signal.h"

// Forward declarations
class SceneTree;

class Camera2D: public Node2D {
    
    public:
        REGISTER_NODE(Camera2D, Node2D, {});

        void init();
        void kill();

        RayCam2D* getCamera() {
            return &camera;
        }

        bool isEnabled();
        void enable();
        void disable();

        void setZoom(float value);
        float getZoom();

        Vector2 getCenter();

    protected:
        void removedFromNode(Node* former_parent_node);
    
        void onParentGlobalPositionChanged(Vector2 old_global_position);
        void onParentGlobalRotationChanged(float old_global_rotation);

    private:
        RayCam2D camera;
};

#endif
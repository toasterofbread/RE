#ifndef INCLUDED_CAMERA3D
#define INCLUDED_CAMERA3D

#include "core/signal.h"
#include "node/types/node_3d.h"
#include "node/types/camera_base.h"

// Forward declarations
class SceneTree;

#ifdef Camera3D
#undef Camera3D
#endif

class Camera3D: public Node3D, public CameraBase {
    
    public:
        REGISTER_NODE_WITH_CONSTRUCTOR(Camera3D, Node3D, {
            CameraBase::registerCameraBase<NodeType>(c);
        }, {
            camera.position = Vector3::ZERO();
            camera.target = Vector3(0, 0, 0);
            camera.up = Vector3(0, 1, 0);
            camera.fovy = 65.0f;
            camera.projection = CAMERA_PERSPECTIVE;
        });

        void ready();
        void kill();

        RayCam3D* getCamera() {
            return &camera;
        }

        Vector3 getCenter();

        bool isEnabled();
        void enable();
        void disable();

        void setTarget(Vector3 value);
        Vector3 getTarget();

        void setZoom(float value);
        float getZoom();
    
        void setPosition(Vector3 value);
        void setRotation(Vector3 value);

    protected:
        void removedFromNode(Node* former_parent_node);

        void onParentGlobalPositionChanged(Vector3 old_global_position);
        void onParentGlobalRotationChanged(Vector3 old_global_rotation);
    
    private:
        RayCam3D camera;
        void updateCamera();

};

#endif
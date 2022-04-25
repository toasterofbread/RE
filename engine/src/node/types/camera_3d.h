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

#define CAM_NEAR 1.0
#define CAM_FAR 1000.0

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

        bool isPointVisibleInFrustum(Vector3 point);

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

        struct Frustum {
            enum FACE {
                TOP, BOTTOM, RIGHT, LEFT, FAR, NEAR
            };
            private:
                struct Plane {
                    Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);
                    // float distance = 0.0f;
                    FACE face;

                    void set(Vector3 _normal) {
                        normal = _normal.cAngle();
                    }

                    auto getDistanceTo(const Vector3 point) {

                        float distance;

                        if (face == FACE::NEAR) {
                            distance = CAM_NEAR - point.z;
                        }
                        else if (face == FACE::FAR) {
                            distance = point.z - CAM_FAR;
                        }
                        else {
                            distance = 0.0f;
                        }

                        return point.dot(normal) - distance;
                    }
                };

            public:
                static const int FACE_COUNT = 6;
                Plane faces[6];

                Frustum() {
                    for (int face = 0; face < FACE_COUNT; face++) {
                        faces[face].face = (FACE)face;
                    }
                }

        };

        Frustum& getFrustum() { return frustum; }
        void updateFrustum();

    protected:
        void removedFromNode(Node* former_parent_node);

        void onParentGlobalPositionChanged(Vector3 old_global_position);
        void onParentGlobalRotationChanged(Vector3 old_global_rotation);
    
    private:
        RayCam3D camera;
        void updateCamera();

        Frustum frustum;
        Matrix matrix;
};

#endif
#ifndef INCLUDED_COLLISION_OBJECT_3D
#define INCLUDED_COLLISION_OBJECT_3D

#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"
#include "core/resource/shape/shape.h"

#include <btBulletCollisionCommon.h>
#include <thread>

// Forward declarations
class PhysicsBody3D;

class CollisionObject3D: public Node3D {

    public:
    
        REGISTER_NODE(CollisionObject3D, Node3D, {});
        
        Signal<> SIGNAL_SHAPE_CHANGED;

        void ready();
        void draw();

        void setPosition(Vector3 value);
        void setRotation(Quaternion value);

        void setShape(shared_ptr<Shape3D> value);
        shared_ptr<Shape3D> getShape();

        bool hasPhysicsShape();
        btCollisionShape* getPhysicsShape();

    protected:

        void onParentGlobalScaleChanged(Vector3 old_global_scale);
        void enteredTree();

    private:

        friend class PhysicsServer;
        friend class PhysicsBody3D;
        friend class Shape3D;

        bool isAttachedToBody() { return attached_body != NULL; }
        void attachToBody(PhysicsBody3D* body);
        void detachFromBody();

        void freePhysicsShape();

        shared_ptr<Shape3D> shape = NULL;
        btCollisionShape* physics_shape = NULL;

        PhysicsBody3D* getAttachedBody() { return attached_body; }
        PhysicsBody3D* attached_body = NULL;

        void onScaleChanged(Vector3 old_scale);

        Vector3 base_scale = Vector3::ONE();
        bool disabled = false; // TODO

        void scaleChanged(Vector3 old_scale);
};

#endif
#endif
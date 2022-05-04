#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"

#include <btBulletCollisionCommon.h>
#include <thread>

// Forward declarations
class PhysicsBody3D;

class CollisionShape3D: public Node3D {

    public:
    
        REGISTER_NODE(CollisionShape3D, Node3D, ({
            c->template registerMethod<Vector3>("setBoxShape", &NodeType::setBoxShape);
        }));
        
        Signal<> SIGNAL_POLYGON_CHANGED;

        void ready();
        void draw();

        void setPosition(Vector3 value);
        void setRotation(Quaternion value);

        bool hasShape();
        btCollisionShape* getShape();

        BroadphaseNativeTypes getType();

        void setBoxShape(Vector3 size);
        void setMeshShape(Mesh& mesh);

    protected:

        void onParentGlobalScaleChanged(Vector3 old_global_scale);
        void enteredTree();

    private:

        // thread gen_thread;
        // void generateMesh(Mesh& mesh);

        friend class PhysicsServer;
        friend class PhysicsBody3D;
        bool isAttachedToBody() { return attached_body != NULL; }
        void attachToBody(PhysicsBody3D* body);
        void detachFromBody();

        void freeShape();

        PhysicsBody3D* getAttachedBody() { return attached_body; }
        PhysicsBody3D* attached_body = NULL;

        // Mesh shape
        // dTriMeshDataID mesh_data = NULL;
        // int* mesh_indices = NULL;

        void onScaleChanged(Vector3 old_scale);

        btCollisionShape* shape = NULL;
        Vector3 base_scale = Vector3::ONE();
        bool disabled = false; // !todo

        void scaleChanged(Vector3 old_scale);
};

#endif
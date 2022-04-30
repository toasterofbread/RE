#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"

#include <ode/ode.h>

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
    
        bool hasShape();
        dGeomID getShape();

        //  The ODE classes are:
        //  *  @li dSphereClass
        //  *  @li dBoxClass
        //  *  @li dCylinderClass
        //  *  @li dPlaneClass
        //  *  @li dRayClass
        //  *  @li dConvexClass
        //  *  @li dGeomTransformClass
        //  *  @li dTriMeshClass
        //  *  @li dSimpleSpaceClass
        //  *  @li dHashSpaceClass
        //  *  @li dQuadTreeSpaceClass
        //  *  @li dFirstUserClass
        //  *  @li dLastUserClass
        int getType();

        void setBoxShape(Vector3 size);
        void setMeshShape(Mesh& mesh, Vector3 scale = Vector3::ONE());

    protected:

        void onParentGlobalScaleChanged(Vector3 old_global_scale);
        void enteredTree();

    private:

        friend class PhysicsBody3D;
        
        bool isAttachedToBody() { return attached_body != NULL; }
        void attachToBody(PhysicsBody3D* body);
        void detachFromBody();

        void freeShape();

        PhysicsBody3D* getAttachedBody() { return attached_body; }
        PhysicsBody3D* attached_body = NULL;

        // Mesh shape
        dTriMeshDataID mesh_data;
        int* mesh_indices = NULL;

        // react::Collider* getCollider() { return collider; }
        // react::Collider* collider = NULL;

        void onScaleChanged(Vector3 old_scale);

        dGeomID shape = NULL;
        Vector3 base_scale = Vector3::ONE();
        bool disabled = false; // !todo

        void scaleChanged(Vector3 old_scale);
};

#endif
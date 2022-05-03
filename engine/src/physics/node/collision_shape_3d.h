#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/signal.h"
#include "engine/src/engine_texture.h"

#include <ode/ode.h>
#include <thread>

// Forward declarations
class PhysicsBody3D;

class CollisionShape3D: public Node3D {

    public:
    
        REGISTER_NODE_WITH_CONSTRUCTOR(CollisionShape3D, Node3D, ({
            c->template registerMethod<Vector3>("setBoxShape", &NodeType::setBoxShape);
        }), {
            Engine::getSingleton()->SIGNAL_PROCESS.connectWithoutArgs(&contacts, &vector<Contact*>::clear);
        });
        
        Signal<> SIGNAL_POLYGON_CHANGED;

        void ready();
        void draw();

        void setPosition(Vector3 value);

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
        void setMeshShape(Mesh& mesh);

        struct Contact {
            dContact* contact;
            CollisionShape3D* A;
            CollisionShape3D* B;
            
            Contact(dContact* c, CollisionShape3D* a, CollisionShape3D* b) {
                contact = c; A = a; B = b;
            }

            Vector3 getPoint() {
                return contact->geom.pos;
            }

            Vector3 getNormal() {
                return contact->geom.normal;
            }
        };
        vector<Contact*> contacts;

    protected:

        void onParentGlobalScaleChanged(Vector3 old_global_scale);
        void enteredTree();

    private:

        thread gen_thread;
        void generateMesh(Mesh& mesh);

        friend class PhysicsServer;
        friend class PhysicsBody3D;
        bool isAttachedToBody() { return attached_body != NULL; }
        void attachToBody(PhysicsBody3D* body);
        void detachFromBody();

        void freeShape();

        PhysicsBody3D* getAttachedBody() { return attached_body; }
        PhysicsBody3D* attached_body = NULL;

        // Mesh shape
        dTriMeshDataID mesh_data = NULL;
        int* mesh_indices = NULL;

        void onScaleChanged(Vector3 old_scale);

        dGeomID shape = NULL;
        Vector3 base_scale = Vector3::ONE();
        bool disabled = false; // !todo

        void scaleChanged(Vector3 old_scale);
};

#endif
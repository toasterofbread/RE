#include "engine/compiler_settings.h"

#if PHYSICS_3D_ENABLED

#include "collision_shape_3d.h"

#include "common/draw.h"
#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/physics_body_3d.h"

void CollisionShape3D::ready() {
    super::ready();
}

void CollisionShape3D::draw() {
    super::draw();

    if (!isGlobalVisible() || shape == NULL) {
        return;
    }

    // switch (getType()) {
    //     case b2Shape::e_polygon: {
            
    //         b2Body* body = getAttachedFixture()->GetBody();
    //         Vector2 body_position = PhysicsServer::phys2World2(body->GetPosition());
    //         shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(shape);

    //         Vector2 scale = Vector2(1, 1);

    //         // if (Node3D* parent_3d = dynamic_cast<Node3D*>(attached_body->getParent())) {
    //         //     scale = parent_3d->getGlobalScale();
    //         // }

    //         // scale = getGlobalScale();

    //         for (int i = 0; i < shape->m_count; i += 1) {
    //             Vector2 start_vertex = (PhysicsServer::phys2World2(shape->m_vertices[i]) * scale).rotated(body->GetAngle()) + body_position;
    //             Vector2 end_vertex = (PhysicsServer::phys2World2(shape->m_vertices[i + 1 == shape->m_count ? 0 : i + 1]) * scale).rotated(body->GetAngle()) + body_position;
    //             Draw::drawLine(start_vertex, end_vertex, Colour::RED());
    //         }
    //     }
    // }
}

void CollisionShape3D::setPosition(Vector3 value) {
    super::setPosition(value);

    if (attached_body) {
        btCompoundShape& container = attached_body->getShapeContainer();

        for (int i = 0; i < container.getNumChildShapes(); i++) {
            if (container.getChildShape(i) == shape) {
                btTransform& transform = container.getChildTransform(i);
                transform.setOrigin(PhysicsServer::world2Phys3(getPosition()));
                container.updateChildTransform(i, transform, false);
                break;
            }
        }
    }
}

void CollisionShape3D::setRotation(Quaternion value) {
    super::setRotation(value);

    if (attached_body) {
        btCompoundShape& container = attached_body->getShapeContainer();

        for (int i = 0; i < container.getNumChildShapes(); i++) {
            if (container.getChildShape(i) == shape) {
                btTransform& transform = container.getChildTransform(i);
                transform.setRotation(getRotation());
                container.updateChildTransform(i, transform, true);
                break;
            }
        }
    }
}

void CollisionShape3D::onParentGlobalScaleChanged(Vector3 old_global_scale) {
    super::onParentGlobalScaleChanged(old_global_scale);

    if (!getUseRelativeScale()) {
        return;
    }

    scaleChanged(old_global_scale);
}

void CollisionShape3D::enteredTree() {
    super::enteredTree();
    scaleChanged(base_scale);
}

void CollisionShape3D::scaleChanged(Vector3 old_scale) {

    if (shape == NULL) {
        return;
    }

    base_scale = getGlobalScale();
    Vector3 multiplier = base_scale / (old_scale * getScale());

    // !todo
    switch (getType()) {
        // case react::CollisionShapeType::CONCAVE_SHAPE: {

        //     shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(shape);
        //     for (int i = 0; i < shape->m_count; i++) {
        //         shape->m_vertices[i].x *= multiplier.x; 
        //         shape->m_vertices[i].y *= multiplier.y; 
        //     }

        //     SIGNAL_POLYGON_CHANGED.emit();

        // }
    }
}

void CollisionShape3D::setBoxShape(Vector3 size) {
    freeShape();

    base_scale = getGlobalScale();
    size = PhysicsServer::world2Phys3(size) * base_scale;

    shape = new btBoxShape(size / 2.0f);
    // shape = new btCapsuleShape(size.x / 2.0f, size.y);
    shape->setUserPointer(this);

    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::setMeshShape(Mesh& mesh) {
    freeShape();

    base_scale = getGlobalScale();

    int* indices = (int*)malloc(mesh.vertexCount * sizeof(int));
    for (int i = 0; i < mesh.vertexCount; i++) {
        indices[i] = i;
    }

    btTriangleIndexVertexArray* interface = new btTriangleIndexVertexArray;
    
    btIndexedMesh part;
    part.m_vertexBase = (const unsigned char*)mesh.vertices;
    part.m_vertexStride = sizeof(float) * 3;
    part.m_numVertices = mesh.vertexCount;
    part.m_triangleIndexBase = (const unsigned char*)indices;
    part.m_triangleIndexStride = sizeof(int) * 3;
    part.m_numTriangles = mesh.triangleCount;

    interface->addIndexedMesh(part);

    shape = new btBvhTriangleMeshShape(interface, false);
    shape->setUserPointer(this);

    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::freeShape() {

    if (shape == NULL) {
        return;
    }

    detachFromBody();

    switch (getType()) {
        case TRIANGLE_MESH_SHAPE_PROXYTYPE: {
            // free(mesh_indices);
            // dGeomTriMeshDataDestroy(mesh_data);
            break;
        }
    }

    delete shape;
    shape = NULL;
}

bool CollisionShape3D::hasShape() {
    return shape != NULL;
}

btCollisionShape* CollisionShape3D::getShape() {
    ASSERT(shape != NULL);
    return shape;
}

BroadphaseNativeTypes CollisionShape3D::getType() {
    ASSERT(shape != NULL);
    return (BroadphaseNativeTypes)shape->getShapeType();
}

void CollisionShape3D::attachToBody(PhysicsBody3D* body) {
    ASSERT(attached_body == NULL);
    ASSERT(shape != NULL);
    attached_body = body;
    body->getShapeContainer().addChildShape(btTransform(getRotation(), PhysicsServer::world2Phys3(getPosition())), shape);
}

void CollisionShape3D::detachFromBody() {
    ASSERT(attached_body != NULL && shape != NULL);
    attached_body->getShapeContainer().removeChildShape(shape);
    attached_body = NULL;
}

#endif
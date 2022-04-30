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
    shape = dCreateBox(PhysicsServer::getSpace(), size.x, size.y, size.z);

    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::setMeshShape(Mesh& mesh, Vector3 scale) {
    freeShape();

    mesh_indices = (int*)malloc(mesh.vertexCount * 3 * sizeof(float));
    for (int i = 0; i < mesh.vertexCount; i++) {
        mesh_indices[i] = i;
    }

    mesh_data = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(mesh_data, mesh.vertices, 3 * sizeof(float), mesh.vertexCount, mesh_indices, mesh.vertexCount, 3 * sizeof(int));
    shape = dCreateTriMesh(PhysicsServer::getSpace(), mesh_data, NULL, NULL, NULL);

    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::freeShape() {

    if (shape == NULL) {
        return;
    }

    dGeomDestroy(shape);
    
    switch (getType()) {
        case dTriMeshClass: {
            free(mesh_indices);
            dGeomTriMeshDataDestroy(mesh_data);
            break;
        }
    }

    shape = NULL;
}

bool CollisionShape3D::hasShape() {
    return shape != NULL;
}

dGeomID CollisionShape3D::getShape() {
    ASSERT(shape != NULL);
    return shape;
}

int CollisionShape3D::getType() {
    ASSERT(shape != NULL);
    return dGeomGetClass(shape);
}

void CollisionShape3D::attachToBody(PhysicsBody3D* body) {
    ASSERT(attached_body == NULL);
    ASSERT(shape != NULL);

    attached_body = body;
    dGeomSetBody(shape, body->getBody());
}

void CollisionShape3D::detachFromBody() {
    ASSERT(attached_body != NULL && shape != NULL);
    attached_body = NULL;
    dGeomSetBody(shape, 0);
}

#endif
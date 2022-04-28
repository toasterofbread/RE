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

    if (!isGlobalVisible() || collision_shape == NULL) {
        return;
    }

    // switch (getType()) {
    //     case b2Shape::e_polygon: {
            
    //         b2Body* body = getAttachedFixture()->GetBody();
    //         Vector2 body_position = PhysicsServer::phys2World2(body->GetPosition());
    //         shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(collision_shape);

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

    if (collider == NULL) {
        return;
    }

    base_scale = getGlobalScale();
    Vector3 multiplier = base_scale / (old_scale * getScale());

    switch (collider->getCollisionShape()->getType()) {
        // case react::CollisionShapeType::CONCAVE_SHAPE: {

        //     shared_ptr<b2PolygonShape> shape = reinterpret_pointer_cast<b2PolygonShape>(collision_shape);
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

    collision_shape = PhysicsServer::getCommon()->createBoxShape(size * 0.5);
    
    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::setMeshShape(Mesh& mesh, Vector3 scale) {
    freeShape();

    react::TriangleVertexArray* array = 
    new react::TriangleVertexArray(mesh.vertexCount, &mesh.vertices[0], 3 * sizeof(float), mesh.triangleCount, 
    &mesh.indices[0], 3 * sizeof(int), 
    react::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, 
    react::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

    react::TriangleMesh* triangleMesh = PhysicsServer::getCommon()->createTriangleMesh(); 
    triangleMesh->addSubpart(array);
    collision_shape = PhysicsServer::getCommon()->createConcaveMeshShape(triangleMesh, scale);
    
    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::freeShape() {

    if (collision_shape == NULL) {
        return;
    }

    switch (getType()) {
        case react::CollisionShapeName::TRIANGLE_MESH: {
            react::ConcaveMeshShape* shape = dynamic_cast<react::ConcaveMeshShape*>(collision_shape);
            react::TriangleMesh* mesh = shape->mTriangleMesh;
            for (int i = 0; i < mesh->getNbSubparts(); i++) {
                delete mesh->getSubpart(i);
            }
            PhysicsServer::getCommon()->destroyTriangleMesh(mesh);
            PhysicsServer::getCommon()->destroyConcaveMeshShape(shape);
            break;
        }
        case react::CollisionShapeName::BOX: {
            PhysicsServer::getCommon()->destroyBoxShape(dynamic_cast<react::BoxShape*>(collision_shape));
        }
    }

    collision_shape = NULL;
}

bool CollisionShape3D::hasShape() {
    return collision_shape != NULL;
}

react::CollisionShape* CollisionShape3D::getShape() {
    ASSERT(collision_shape != NULL);
    return collision_shape;
}

react::CollisionShapeName CollisionShape3D::getType() {
    ASSERT(collision_shape != NULL);
    return collision_shape->getName();
}

void CollisionShape3D::attachToBody(PhysicsBody3D* body, react::Collider* _collider) {
    ASSERT(attached_body == NULL);
    attached_body = body;
    collider = _collider;
}

void CollisionShape3D::detachFromBody() {
    ASSERT(attached_body != NULL && collision_shape != NULL);
    attached_body = NULL;
    collision_shape = NULL;
}

#endif
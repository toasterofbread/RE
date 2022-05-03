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

    if (shape != NULL && dGeomGetBody(shape)) {
        value = PhysicsServer::world2Phys3(value);
        dGeomSetOffsetPosition(shape, value.x, value.y, value.z);
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

enum INDEX
{
  PLANE = 0,
  PLAYER,
  OBJS,
  PLAYER_BULLET,
  ALL,
  LAST_INDEX_CNT
};

const int catBits[LAST_INDEX_CNT] =
{
    0x0001, ///< Plane category >          0001
    0x0002, ///< Player category >         0010
    0x0004, ///< Objects category >        0100
    0x0008, ///< Player bullets category > 1000
    ~0L     ///< All categories >          11111111111111111111111111111111
};

void CollisionShape3D::setBoxShape(Vector3 size) {
    freeShape();

    base_scale = getGlobalScale();
    size = PhysicsServer::world2Phys3(size) * base_scale;

    shape = dCreateCapsule(PhysicsServer::getSpace(), size.x / 2.0f, size.y);
    // shape = dCreateBox(PhysicsServer::getSpace(), size.x, size.y, size.z);
    dGeomSetData(shape, this);

    dGeomSetCategoryBits(shape, catBits[PLAYER]);
    dGeomSetCollideBits(shape, catBits[PLANE]);

    SIGNAL_POLYGON_CHANGED.emit();
}

void CollisionShape3D::setMeshShape(Mesh& mesh) {
    // freeShape();

    if (gen_thread.joinable()) {
        gen_thread.join();
    }
    
    generateMesh(mesh);
    // gen_thread = thread(&CollisionShape3D::generateMesh, this, ref(mesh));
}

void CollisionShape3D::generateMesh(Mesh& mesh) {

    if (shape != NULL) {
        return;
    }

    dGeomID old_shape = shape;
    int* old_indices = mesh_indices;
    dTriMeshDataID old_data = mesh_data;

    int* new_indices = (int*)malloc(mesh.vertexCount * sizeof(int));
    for (int i = 0; i < mesh.vertexCount; i++ ) {
        new_indices[i] = i;
    }

    dTriMeshDataID new_mesh_data = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(new_mesh_data, mesh.vertices, 3 * sizeof(float), mesh.vertexCount, new_indices, mesh.vertexCount, 3 * sizeof(int));
    
    shape = dCreateTriMesh(PhysicsServer::getSpace(), new_mesh_data, NULL, NULL, NULL);
    dGeomSetCategoryBits(shape, catBits[PLANE]);
    dGeomSetCollideBits(shape, catBits[ALL]);
    dGeomSetData(shape, this);

    mesh_indices = new_indices;
    mesh_data = new_mesh_data;
    
    if (old_shape != NULL)
        dGeomDestroy(old_shape);
    if (old_indices != NULL)
        free(old_indices);
    if (old_data != NULL)
        dGeomTriMeshDataDestroy(old_data);
    
    this->SIGNAL_POLYGON_CHANGED.emit();
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

    Vector3 position = PhysicsServer::world2Phys3(getPosition());
    dGeomSetOffsetPosition(shape, position.x, position.y, position.z);
}

void CollisionShape3D::detachFromBody() {
    ASSERT(attached_body != NULL && shape != NULL);
    attached_body = NULL;
    dGeomSetBody(shape, 0);
}

#endif
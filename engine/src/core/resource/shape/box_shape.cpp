#include "box_shape.h"

#include "physics/node/collision_object_3d.h"

unordered_map<BoxShape3D::HashableBoundingBox, weak_ptr<BoxShape3D>, BoxShape3D::Hasher> BoxShape3D::box_pool;

void BoxShape3D::applyToCollisionObject(CollisionObject3D* object) {
    ASSERT(!size.isZero());
    if (getCollisionShape() == NULL)
        createCollisionShape();
    setAsObjectPhysicsShape(object);
}

void BoxShape3D::createCollisionShape() {
    setCollisionShape(new btBoxShape(size / 2.0f));
}

void BoxShape3D::setSize(const Vector3 value) {
    if (size == value)
        return;

    Vector3 diff = value / size;
    size = value;
    
    ASSERT(size.isUnsigned() && !size.isZero());

    if (getCollisionShape())
        getCollisionShape()->setLocalScaling(getCollisionShape()->getLocalScaling() * diff);
    
    SIGNAL_CHANGED.emit();
}

void BoxShape3D::setSize(const BoundingBox& box) {
    setSize(Vector3(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z));
}

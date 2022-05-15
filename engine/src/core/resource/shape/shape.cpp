#include "shape.h"

#include "physics/node/collision_object_3d.h"

void Shape3D::setAsObjectPhysicsShape(CollisionObject3D* object) {
    object->freePhysicsShape();
    object->physics_shape = collision_shape;
}
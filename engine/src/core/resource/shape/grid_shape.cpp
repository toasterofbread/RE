#include "grid_shape.h"

#include "physics/node/collision_object_3d.h"

#define COORDS unsigned int x, unsigned int y, unsigned int z

void BaseGridShape3D::applyToCollisionObject(CollisionObject3D* object) {
    if (getCollisionShape() == NULL)
        createCollisionShape();
    setAsObjectPhysicsShape(object);
}

void BaseGridShape3D::createCollisionShape() {
    btCompoundShape* col = new btCompoundShape;
    setCollisionShape(col);

    for (auto& [x, y, z] : getSize()) {
        shared_ptr<Shape3D> shape = getShape(x, y, z);
        if (shape == NULL)
            continue;

        if (shape->getCollisionShape() == NULL)
            shape->createCollisionShape();
        
        col->addChildShape(btTransform(btQuaternion::getIdentity(), Vector3(x, y, z)), shape->getCollisionShape());
        shape->SIGNAL_CHANGED.connect(col, &btCompoundShape::recalculateLocalAabb);
    }
}

void BaseGridShape3D::addShape(COORDS, shared_ptr<Shape3D> shape) {

    if (getCollisionShape() != NULL) {
        if (shape->getCollisionShape() == NULL)
            shape->createCollisionShape();

        setShape(x, y, z, shape);
        dynamic_cast<btCompoundShape*>(getCollisionShape())->addChildShape(btTransform(btQuaternion::getIdentity(), Vector3(x, y, z)), shape->getCollisionShape());
        
        if (shape->SIGNAL_CHANGED.getConnectionCount(this) == 0)
            shape->SIGNAL_CHANGED.connect(dynamic_cast<btCompoundShape*>(getCollisionShape()), &btCompoundShape::recalculateLocalAabb);
        
        SIGNAL_CHANGED.emit();
    }
    else {
        setShape(x, y, z, shape);
    }
}

void BaseGridShape3D::removeShape(COORDS) {

    // shape->SIGNAL_CHANGED.disconnect(this);
    
    btCompoundShape* col = (btCompoundShape*)getCollisionShape();

    for (int i = 0; i < col->getNumChildShapes(); i++) {
        if (((Vector3)col->getChildTransform(i).getOrigin()).equals(x, y, z)) {
            col->removeChildShapeByIndex(i);
            SIGNAL_CHANGED.emit();
            break;
        }
    }

    setShape(x, y, z, NULL);
}

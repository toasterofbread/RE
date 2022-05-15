#ifndef INCLUDED_GRIDSHAPE
#define INCLUDED_GRIDSHAPE

#include "shape.h"

#define COORDS unsigned int x, unsigned int y, unsigned int z

class BaseGridShape3D: public Shape3D {

    public:
        void applyToCollisionObject(CollisionObject3D* object);
        void createCollisionShape();
        void addShape(COORDS, shared_ptr<Shape3D> shape);
        void removeShape(COORDS);

        TYPE getType() { return TYPE::GRID; }

    private:
        virtual Vector3 getSize() = 0;
        virtual void setShape(COORDS, shared_ptr<Shape3D> shape) = 0;
        virtual shared_ptr<Shape3D> getShape(COORDS) = 0;

};

template<uint16_t size_x, uint16_t size_y, uint16_t size_z>
class GridShape3D: public BaseGridShape3D {
    private:
        shared_ptr<Shape3D> shapes[size_x][size_y][size_z];

        void setShape(COORDS, shared_ptr<Shape3D> shape) {
            auto a = size_x;
            auto b = size_y;
            auto c = size_z;
            shapes[x][y][z] = shape;
        }

        shared_ptr<Shape3D> getShape(COORDS) {
            return shapes[x][y][z];
        }

        Vector3 getSize() {
            return Vector3(size_x, size_y, size_z);
        }
};

#undef COORDS

#endif
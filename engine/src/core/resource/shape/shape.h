#ifndef INCLUDED_SHAPE
#define INCLUDED_SHAPE

#include "../resource.h"

#include "core/signal.h"

// Forward declarations
class CollisionObject2D;
class CollisionObject3D;
class BaseGridShape3D;

class Shape {
    protected:
        Shape() {};
    public:
        template<typename ShapeType>
        static shared_ptr<ShapeType> create() { 
            shared_ptr<ShapeType> ret = shared_ptr<ShapeType>(new ShapeType); 
            ret->type = ret->getType();
            return ret; 
        }
        Signal<> SIGNAL_CHANGED;
};

class Shape2D: public Shape {

    public:
        enum class TYPE {
            NONE
        };

        virtual void applyToCollisionObject(CollisionObject2D* object) = 0; 
        virtual TYPE getType() = 0;

    private:
        friend class Shape;
        TYPE type = TYPE::NONE;

};

class Shape3D: public Shape {

    public:
        enum class TYPE {
            NONE, BOX, GRID
        };

        ~Shape3D() {
            if (collision_shape)
                delete collision_shape;
        }

        virtual void applyToCollisionObject(CollisionObject3D* object) = 0;
        virtual void createCollisionShape() = 0;
        virtual TYPE getType() = 0;

        btCollisionShape* getCollisionShape() { return collision_shape; }

    protected:
        void setAsObjectPhysicsShape(CollisionObject3D* object);

        friend class Shape;
        TYPE type = TYPE::NONE;

        void setCollisionShape(btCollisionShape* shape) {
            collision_shape = shape;
        }
    
    private:
        btCollisionShape* collision_shape = NULL;

};

#endif
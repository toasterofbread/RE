#ifndef INCLUDED_BOXSHAPE
#define INCLUDED_BOXSHAPE

#include "shape.h"

class BoxShape3D: public Shape3D {
    public:
        void applyToCollisionObject(CollisionObject3D* object);
        void createCollisionShape();

        void setSize(const Vector3 value);
        void setSize(const BoundingBox& box);
        Vector3 getSize();

        TYPE getType() { return TYPE::BOX; }

        static shared_ptr<BoxShape3D> getFromBoundingBox(const BoundingBox& box) {
            
            if (box_pool.count(box) && !box_pool[box].expired()) {
                weak_ptr<BoxShape3D> ret = box_pool[box];
                if (!ret.expired())
                    return box_pool[box].lock();
            }

            shared_ptr<BoxShape3D> ret = create<BoxShape3D>();
            ret->setSize(box);
            box_pool[box] = ret;

            return ret;
        }

    private:
        Vector3 size = Vector3::ZERO();
        
        struct HashableBoundingBox: public BoundingBox {
            HashableBoundingBox(const BoundingBox& box) {
                max = box.max;
                min = box.min;
            }
            bool operator==(const HashableBoundingBox& other) const {
                return (Vector3)min == other.min && (Vector3)max == other.max;
            }
        };

        struct Hasher {
            size_t operator()(const BoundingBox& box) const {
                size_t ret = 17;
                ret = ret * 31 + hash<float>()(box.min.x);
                ret = ret * 31 + hash<float>()(box.min.y);
                ret = ret * 31 + hash<float>()(box.min.z);
                ret = ret * 31 + hash<float>()(box.max.x);
                ret = ret * 31 + hash<float>()(box.max.y);
                ret = ret * 31 + hash<float>()(box.max.z);
                return ret;
            }
        };

        static unordered_map<HashableBoundingBox, weak_ptr<BoxShape3D>, Hasher> box_pool;
};

#endif
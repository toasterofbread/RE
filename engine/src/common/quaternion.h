#ifndef INCLUDED_QUATERNION
#define INCLUDED_QUATERNION

#include "engine/compiler_settings.h"
#include "common/vector3.h"
#include "raylib/raymath.h"

#include <json.hpp>
#include "common/raylib.h"
#include <string>
using namespace std;
using json = nlohmann::json;

#if PHYSICS_3D_ENABLED
#include <btBulletCollisionCommon.h>
#endif

struct InternalQuaternion: public Quaternion {

    InternalQuaternion() {}
    InternalQuaternion(Quaternion quat) { set(quat); clean(); }
    InternalQuaternion(Vector3 v, float _w) { x = v.x; y = v.y; z = v.z; w = _w; clean(); }
    InternalQuaternion(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; clean(); }

    void set(Quaternion quat) {
        x = quat.x;
        y = quat.y;
        z = quat.z;
        w = quat.w;
    }

    string toString(int max_decimals = -1) const {
        return Vector3(x, y, z).toString(max_decimals) + " | " + to_string(w);
    }

    bool isValid() const {
        return !isnan(x) && !isnan(y) && !isnan(z) && !isnan(w);
    }

    void invert() {
        x *= -1;
        y *= -1;
        z *= -1;
    }

    InternalQuaternion inverted() const {
        InternalQuaternion ret = InternalQuaternion(x, y, z, w);
        ret.invert();
        return ret;
    }

    void normalise() {
        set(normalised());
    }

    InternalQuaternion normalised() {
        InternalQuaternion ret;

        float length = sqrtf(x * x + y * y + z * z + w * w);
        if (length == 0.0f) 
            length = 1.0f;
        
        float ilength = 1.0f/length;

        ret.x = x * ilength;
        ret.y = y * ilength;
        ret.z = z * ilength;
        ret.w = w * ilength;

        return ret;
    }

    InternalQuaternion getAxisComponent(Vector3 axis) {
        float dot = axis.dot(vector());
        axis *= dot;
        
        InternalQuaternion ret = InternalQuaternion(axis.x, axis.y, axis.z, w).normalised();
        
        if (dot < 0.0f) {
            ret.x = -ret.x;
            ret.y = -ret.y;
            ret.z = -ret.z;
            ret.w = -ret.w;
        }
        
        return ret;
    }

    Vector3 vector() const {
        return Vector3(x, y, z);
    }

    void clean() {
        if (x == -0)
            x = 0;
        if (y == -0)
            y = 0;
        if (z == -0)
            z = 0;
        if (w == -0)
            w = 0;
    }

    #define APPREQ(a, b) (abs(a - b) < 0.0002)
    bool operator==(const InternalQuaternion& other) {
        return APPREQ(x, other.x) && APPREQ(y, other.y) && APPREQ(z, other.z) && APPREQ(w, other.w);
    }

    bool operator!=(const InternalQuaternion& other) {
        return !APPREQ(x, other.x) || !APPREQ(y, other.y) || !APPREQ(z, other.z) || !APPREQ(w, other.w);
    }
    #undef APPREQ

    static InternalQuaternion IDENTITY() {
        return QuaternionIdentity();
    }

    #if PHYSICS_3D_ENABLED
    InternalQuaternion(const btQuaternion& quat) {
        x = quat.getX();
        y = quat.getY();
        z = quat.getZ();
        w = quat.getW();
    }
    operator btQuaternion() {
        return btQuaternion(x, y, z, w);
    }
    #endif

};

#ifdef Quaternion
#undef Quaternion
#endif
#define Quaternion InternalQuaternion

#endif
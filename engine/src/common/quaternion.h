#ifndef INCLUDED_QUATERNION
#define INCLUDED_QUATERNION

#include "engine/compiler_settings.h"
#include "common/vector2.h"
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
    InternalQuaternion(Quaternion quat) { x = quat.x; y = quat.y; z = quat.z; w = quat.w; }
    InternalQuaternion(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

    string toString() const {
        return Vector3(x, y, z).toString() + " | " + to_string(w);
    }

    bool equals(float _x, float _y, float _z) {
        return x == _x && y == _y && z == _z;
    }
    bool operator==(const ::InternalQuaternion& other) {
        return x == other.x && y == other.y && z == other.z;
    }

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
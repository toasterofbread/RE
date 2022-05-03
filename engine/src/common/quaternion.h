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

// #if PHYSICS_3D_ENABLED
// #include <ode/ode.h>
// #endif

struct InternalQuaternion: public Quaternion {

    InternalQuaternion() {}
    InternalQuaternion(Quaternion quat) { x = quat.x; y = quat.y; z = quat.z; w = quat.w; }
    InternalQuaternion(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

    bool equals(float _x, float _y, float _z) {
        return x == _x && y == _y && z == _z;
    }
    bool operator==(const ::InternalQuaternion& other) {
        return x == other.x && y == other.y && z == other.z;
    }

    static InternalQuaternion IDENTITY() {
        return QuaternionIdentity();
    }
};

#ifdef Quaternion
#undef Quaternion
#endif
#define Quaternion InternalQuaternion

#endif
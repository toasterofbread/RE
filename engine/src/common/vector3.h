#ifndef INCLUDED_VECTOR3
#define INCLUDED_VECTOR3

#include "engine/compiler_settings.h"
#include "common/vector2.h"
#include "raylib/raymath.h"

#include <json.hpp>
#include "common/raylib.h"
#include <box2d/box2d.h>
#include <string>
using namespace std;
using json = nlohmann::json;

struct InternalVector3: public Vector3 {


    InternalVector3(): Vector3() {}
    InternalVector3(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
    InternalVector3(int _x, int _y, int _z) {x = _x; y = _y; z = _z;}
    InternalVector3(int _x, float _y, float _z) {x = _x; y = _y; z = _z;}
    InternalVector3(float _x, int _y, int _z) {x = _x; y = _y; z = _z;}
    InternalVector3(float _x, int _y, float _z) {x = _x; y = _y; z = _z;}
    InternalVector3(int _x, int _y, float _z) {x = _x; y = _y; z = _z;}
    InternalVector3(int _x, float _y, int _z) {x = _x; y = _y; z = _z;}
    InternalVector3(Vector3 vector) {x = vector.x; y = vector.y; z = vector.z;}

    static InternalVector3 fromJson(json data);

    static InternalVector3 from(float value) {
        return InternalVector3(value, value, value);
    }

    bool compare(float cx, float cy, float cz) {
        return x == cx && y == cy && z == cz;
    }

    string toString();

    InternalVector3 deg2rad();
    InternalVector3 rad2deg();

    InternalVector3 ABS() {
        return InternalVector3(abs(x), abs(y), abs(z));
    }

    InternalVector3 clampAngle();

    InternalVector3 move(Vector2 direction, InternalVector3 rotation, float delta);

    float dot(InternalVector3 with) {
        return Vector3DotProduct(*this, with);
    }

    // void rotate(float angle) {
    //     float _x = x;
    //     float _y = y;

    //     x = _x*cosf(angle) - _y*sinf(angle);
    //     y = _x*sinf(angle) + _y*cosf(angle);
    // }

    // void rotateAround(float angle, InternalVector3 origin) {
    //     float _x = x;
    //     float _y = y;

    //     x = cos(angle) * (_x - origin.x) - sin(angle) * (_y - origin.y) + origin.x;
    //     y = sin(angle) * (_x - origin.x) + cos(angle) * (_y - origin.y) + origin.y;
    // }

    // InternalVector3 rotated(float angle) {
    //     InternalVector3 ret = InternalVector3(x, y);
    //     ret.rotate(angle);
    //     return ret;
    // }

    // InternalVector3 rotatedAround(float angle, InternalVector3 origin) {
    //     InternalVector3 ret = InternalVector3(x, y);
    //     ret.rotateAround(angle, origin);
    //     return ret;
    // }

    bool isZero() {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    void applyDeadzone(float deadzone) {
        if (abs(x) <= deadzone) {
            x = 0.0f;
        }
        if (abs(y) <= deadzone) {
            y = 0.0f;
        }
        if (abs(z) <= deadzone) {
            z = 0.0f;
        }
    }

    InternalVector3& operator=(const ::InternalVector3& vector) {
        x = vector.x;
        y = vector.y;
        z = vector.z;
        return *this;
    }

    bool operator==(const ::InternalVector3& other) {
        return x == other.x && y == other.y && z == other.z;
    }

    InternalVector3 operator+(const ::InternalVector3& vector) const {
        return InternalVector3(x + vector.x, y + vector.y, z + vector.z);
    }

    InternalVector3 operator-(const ::InternalVector3& vector) const {
        return InternalVector3(x - vector.x, y - vector.y, z - vector.z);
    }

    InternalVector3 operator-() const {
        return InternalVector3(-x, -y, -z);
    }

    InternalVector3 operator*(const ::InternalVector3& vector) const {
        return InternalVector3(x * vector.x, y * vector.y, z * vector.z);
    }

    InternalVector3 operator*(const float scale) const {
        return InternalVector3(x * scale, y * scale, z * scale);
    }

    InternalVector3 operator/(const ::InternalVector3& vector) const {
        return InternalVector3(x / vector.x, y / vector.y, z / vector.z);
    }

    InternalVector3 operator/(const float div) const {
        return InternalVector3(x / div, y / div, z / div);
    }

    InternalVector3& operator+=(const ::InternalVector3& vector) {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }

    InternalVector3& operator-=(const ::InternalVector3& vector) {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }

    InternalVector3& operator*=(const ::InternalVector3& vector) {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        return *this;
    }

    InternalVector3& operator*=(const float scale) {
        x *= scale;
        y *= scale;
        z *= scale;
        return *this;
    }

    InternalVector3& operator/=(const ::InternalVector3& vector) {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        return *this;
    }

    InternalVector3& operator/=(const float div) {
        x /= div;
        y /= div;
        z /= div;
        return *this;
    }
    
    static InternalVector3 ZERO() {
        return InternalVector3(0.0f, 0.0f, 0.0f);
    }

    static InternalVector3 ONE() {
        return InternalVector3(1.0f, 1.0f, 1.0f);
    }

    // operator Vector3() {
    //     return Vector3{x, y};
    // }

};

#ifdef Vector3
#undef Vector3
#endif
#define Vector3 InternalVector3

#endif
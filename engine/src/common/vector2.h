#ifndef INCLUDED_VECTOR2
#define INCLUDED_VECTOR2

#include "engine/compiler_settings.h"

#include <box2d/box2d.h>
#include <string>
using namespace std;

#if PLATFORM == PLATFORM_RAYLIB
#include "raylib_include.h"
#endif

struct InternalVector2: public b2Vec2 {
    InternalVector2(): b2Vec2(0.0f, 0.0f) {}
    InternalVector2(float x, float y): b2Vec2(x, y) {}

    string toString() {
        return "{ " + to_string(x) + ", " + to_string(y) + " }";
    }

    void rotate(float angle) {
        float _x = x;
        float _y = y;

        x = _x*cosf(angle) - _y*sinf(angle);
        y = _x*sinf(angle) + _y*cosf(angle);
    }

    InternalVector2 rotated(float angle) {
        InternalVector2 ret = InternalVector2(x, y);
        ret.rotate(angle);
        return ret;
    }

    bool isZero() {
        return x == 0.0f && y == 0.0f;
    }

    void applyDeadzone(float deadzone) {
        if (abs(x) <= deadzone) {
            x = 0.0f;
        }
        if (abs(y) <= deadzone) {
            y = 0.0f;
        }
    }

    InternalVector2& operator=(const ::InternalVector2& vector2) {
        Set(vector2.x, vector2.y);
        return *this;
    }

    bool operator==(const ::InternalVector2& other) {
        return x == other.x && y == other.y;
    }

    InternalVector2 operator+(const ::InternalVector2& vector2) const {
        return InternalVector2(x + vector2.x, y + vector2.y);
    }

    InternalVector2 operator-(const ::InternalVector2& vector2) const {
        return InternalVector2(x - vector2.x, y - vector2.y);
    }

    InternalVector2 operator-() const {
        return InternalVector2(-x, -y);
    }

    InternalVector2 operator*(const ::InternalVector2& vector2) const {
        return InternalVector2(x * vector2.x, y * vector2.y);
    }

    InternalVector2 operator*(const float scale) const {
        return InternalVector2(x * scale, y * scale);
    }

    InternalVector2 operator/(const ::InternalVector2& vector2) const {
        return InternalVector2(x / vector2.x, y / vector2.y);
    }

    InternalVector2 operator/(const float div) const {
        return InternalVector2(x / div, y / div);
    }

    InternalVector2& operator+=(const ::InternalVector2& vector2) {
        x += vector2.x;
        y += vector2.y;
        return *this;
    }

    InternalVector2& operator-=(const ::InternalVector2& vector2) {
        x -= vector2.x;
        y -= vector2.y;
        return *this;
    }

    InternalVector2& operator*=(const ::InternalVector2& vector2) {
        x *= vector2.x;
        y *= vector2.y;
        return *this;
    }

    InternalVector2& operator*=(const float scale) {
        x *= scale;
        y *= scale;
        return *this;
    }

    InternalVector2& operator/=(const ::InternalVector2& vector2) {
        x /= vector2.x;
        y /= vector2.y;
        return *this;
    }

    InternalVector2& operator/=(const float div) {
        x /= div;
        y /= div;
        return *this;
    }
    
    #if PLATFORM == PLATFORM_RAYLIB
    operator raylib::Vector2() {
        return raylib::Vector2{x, y};
    }
    #endif

};

#define Vector2 InternalVector2

#define VECTOR2ZERO Vector2(0.0f, 0.0f)
#define VECTOR2ONE Vector2(1.0f, 1.0f)

#endif
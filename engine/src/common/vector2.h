#ifndef INCLUDED_VECTOR2
#define INCLUDED_VECTOR2

#include "engine/compiler_settings.h"

#include <nlohmann/json.hpp>
#include "common/raylib.h"
#include <box2d/box2d.h>
#include <string>
using namespace std;
using json = nlohmann::json;

#undef Vector2

struct InternalVector2: public b2Vec2 {
    InternalVector2(): b2Vec2(0.0f, 0.0f) {}
    InternalVector2(float x, float y): b2Vec2(x, y) {}
    InternalVector2(int x, int y): b2Vec2(x, y) {}
    InternalVector2(int x, float y): b2Vec2(x, y) {}
    InternalVector2(float x, int y): b2Vec2(x, y) {}
    InternalVector2(b2Vec2 vector): b2Vec2(vector.x, vector.y) {}
    InternalVector2(Vector2 vector): b2Vec2(vector.x, vector.y) {}
    
    static InternalVector2 fromJson(json data);

    static InternalVector2 from(float value) {
        return InternalVector2(value, value);
    }

    bool compare(float cx, float cy) {
        return x == cx && y == cy;
    }

    float dot(InternalVector2 with) {
        return b2Dot(*this, with);
    }

    InternalVector2 ABS() {
        return InternalVector2(abs(x), abs(y));
    }

    string toString(int max_decimals = 1);

    void rotate(float angle) {
        float _x = x;
        float _y = y;

        x = _x*cosf(angle) - _y*sinf(angle);
        y = _x*sinf(angle) + _y*cosf(angle);
    }

    void rotateAround(float angle, InternalVector2 origin) {
        float _x = x;
        float _y = y;

        x = cos(angle) * (_x - origin.x) - sin(angle) * (_y - origin.y) + origin.x;
        y = sin(angle) * (_x - origin.x) + cos(angle) * (_y - origin.y) + origin.y;
    }

    InternalVector2 rotated(float angle) {
        InternalVector2 ret = InternalVector2(x, y);
        ret.rotate(angle);
        return ret;
    }

    InternalVector2 rotatedAround(float angle, InternalVector2 origin) {
        InternalVector2 ret = InternalVector2(x, y);
        ret.rotateAround(angle, origin);
        return ret;
    }

    void normalise() {
        float length = sqrtf((x*x) + (y*y));
        if (length > 0) {
            x = x * 1.0f / length;
            y = y * 1.0f / length;
        }
    }

    InternalVector2 normalised() {
        InternalVector2 ret = InternalVector2(x, y);
        ret.normalise();
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
    
    static InternalVector2 ZERO() {
        return InternalVector2(0.0f, 0.0f);
    }

    static InternalVector2 ONE() {
        return InternalVector2(1.0f, 1.0f);
    }

    static InternalVector2 UP() {
        return InternalVector2(0, -1);
    }

    static InternalVector2 DOWN() {
        return InternalVector2(0, 1);
    }

    static InternalVector2 LEFT() {
        return InternalVector2(-1, 0);
    }

    static InternalVector2 RIGHT() {
        return InternalVector2(1, 0);
    }

    operator Vector2() {
        return Vector2{x, y};
    }

};

#ifdef Vector2
#undef Vector2
#endif
#define Vector2 InternalVector2

#endif
#ifndef INCLUDED_COLOUR
#define INCLUDED_COLOUR

#include <box2d/box2d.h>
#include "engine/compiler_settings.h"
#include "common/raylib.h"

struct Colour: public b2Color {

    Colour(float r, float g, float b, float a = 1.0f): b2Color(r, g, b, a) {}

    unsigned int toInt() {
        return (((((int)(a * 255.0f))&0xFF)<<24) | ((((int)(b * 255.0f))&0xFF)<<16) | ((((int)(g * 255.0f))&0xFF)<<8) | ((((int)(r * 255.0f))&0xFF)<<0));
    }

    Colour operator*(const Colour &colour) const {
        return Colour(r * colour.r, g * colour.g, b * colour.b, a * colour.a);
    }

    void operator*=(const Colour &colour) {
        r *= colour.r;
        g *= colour.g;
        b *= colour.b;
        a *= colour.a;
    }

    Colour operator/(const Colour &colour) const  {
        return Colour(r / colour.r, g / colour.g, b / colour.b, a / colour.a);
    }

    void operator/=(const Colour &colour) {
        r /= colour.r;
        g /= colour.g;
        b /= colour.b;
        a /= colour.a;
    }

    static Colour RED() {
        return Colour(1, 0, 0);
    }
    static Colour GREEN() {
        return Colour(0, 1, 0);
    }
    static Colour BLUE() {
        return Colour(0, 0, 1);
    }
    static Colour BLACK() {
        return Colour(0, 0, 0);
    }
    static Colour WHITE() {
        return Colour(1, 1, 1);
    }
    static Colour TRANSPARENT() {
        return Colour(1, 1, 1, 0);
    }

    operator Color() {
        Color ret;
        ret.r = r * 255;
        ret.g = g * 255;
        ret.b = b * 255;
        ret.a = a * 255;
        return ret;
    }

};

#endif
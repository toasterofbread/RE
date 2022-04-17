#ifndef INCLUDED_COLOUR
#define INCLUDED_COLOUR

#include <box2d/box2d.h>
#include "engine/compiler_settings.h"

#if PLATFORM == PLATFORM_RAYLIB
#include "raylib_include.h"
#endif

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

    #if PLATFORM == PLATFORM_RAYLIB
    operator Color() {
        raylib::Color ret;
        ret.r = r * 255;
        ret.g = g * 255;
        ret.b = b * 255;
        ret.a = a * 255;
        return ret;
    }
    #endif

};

#endif

#undef RED
#undef GREEN
#undef BLUE
#undef BLACK
#undef WHITE
#undef TRANSPARENT

#define RED Colour(1, 0, 0)
#define GREEN Colour(0, 1, 0)
#define BLUE Colour(0, 0, 1)
#define BLACK Colour(0, 0, 0)
#define WHITE Colour(1, 1, 1)
#define TRANSPARENT Colour(1, 1, 1, 0)
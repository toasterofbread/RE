#ifndef INCLUDED_RAYLIB
#define INCLUDED_RAYLIB

#include <raylib-cpp.hpp>

#include <iostream>

#define Vector2 raylib::Vector2
#define ZEROVECTOR2 Vector2(0, 0)

class Colour: public Color {
    public:
        Colour(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }
        Colour() {};

        Colour operator*(const Colour &colour) const {
            return Colour(
                ((r / 255.0f) * (colour.r / 255.0f)) * 255.0f,
                ((g / 255.0f) * (colour.g / 255.0f)) * 255.0f,
                ((b / 255.0f) * (colour.b / 255.0f)) * 255.0f,
                ((a / 255.0f) * (colour.a / 255.0f)) * 255.0f
            );
        }

        void operator*=(const Colour &colour) {
            r = ((r / 255.0f) * (colour.r / 255.0f)) * 255.0f;
            g = ((g / 255.0f) * (colour.g / 255.0f)) * 255.0f;
            b = ((b / 255.0f) * (colour.b / 255.0f)) * 255.0f;
            a = ((a / 255.0f) * (colour.a / 255.0f)) * 255.0f;
        }

        Colour operator/(const Color &colour) const  {
            return Colour(
                ((r / 255.0f) / (colour.r / 255.0f)) * 255.0f,
                ((g / 255.0f) / (colour.g / 255.0f)) * 255.0f,
                ((b / 255.0f) / (colour.b / 255.0f)) * 255.0f,
                ((a / 255.0f) / (colour.a / 255.0f)) * 255.0f
            );
        }

        void operator/=(const Colour &colour) {
            r = ((r / 255.0f) / (colour.r / 255.0f)) * 255.0f;
            g = ((g / 255.0f) / (colour.g / 255.0f)) * 255.0f;
            b = ((b / 255.0f) / (colour.b / 255.0f)) * 255.0f;
            a = ((a / 255.0f) / (colour.a / 255.0f)) * 255.0f;
        }

        // !todo
        // std::ostream operator<<(std::ostream& out) {
        //     return out << "(";
        // }
        
};

#define Color Colour

#endif


#include "common/draw.h"

#include "raylib_include.h"

// #include "common/colour.h"
// #include "common/vector2.h"

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour) {
    DrawLine(start_x, start_y, end_x, end_y, Colour{colour.r, colour.g, colour.b, colour.a});
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour) {
    DrawLineV(start, end, Colour{colour.r, colour.g, colour.b, colour.a});
    // DrawLine(start.x, start.y, end_x, end_y, raylib::Color{colour.r, colour.g, colour.b, colour.a});
}
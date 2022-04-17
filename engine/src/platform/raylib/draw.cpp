#include "common/draw.h"

#include "raylib_include.h"

#include "common/colour.h"
#include "common/vector2.h"
#include "core/node/node_types/camera.h"
#include "engine.h"

void Draw::init() {

}

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, DRAW_MODE mode) {
    Vector2 offset = getDrawOffset(mode);
    DrawLine(start_x + offset.x, start_y + offset.y, end_x + offset.x, end_y + offset.y, colour);
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour, DRAW_MODE mode) {
    drawLine(start.x, start.y, end.x, end.y, colour, mode);
}

void Draw::drawTextureRST(TEXTURE_TYPE texture, Vector2 position, float rotation, Vector2 scale, Colour tint, DRAW_MODE mode) {
    
    Vector2 offset = getDrawOffset(mode);
    
    DrawTexturePro(
        texture,
        Rectangle{
            0,
            0,
            (float)OS::getTextureWidth(texture),// * (flip_x ? -1.0f : 1.0f),
            (float)OS::getTextureHeight(texture)// * (flip_y ? -1.0f : 1.0f)
        },
        Rectangle{
            position.x + offset.x,
            position.y + offset.y,
            (float)OS::getTextureWidth(texture) * scale.x,
            (float)OS::getTextureHeight(texture) * scale.y
        },
        VECTOR2ZERO,
        rotation * RAD2DEG,
        tint
    );
}

void Draw::drawText(string text, float pos_x, float pos_y, Colour colour, float size, DRAW_MODE mode) {
    Vector2 offset = getDrawOffset(mode);
    DrawText(text.c_str(), pos_x + offset.x, pos_y + offset.y, size * 10.0f, colour);
}
void Draw::drawText(string text, Vector2 position, Colour colour, float size, DRAW_MODE mode) {
    drawText(text, position.x, position.y, colour, size, mode);
}
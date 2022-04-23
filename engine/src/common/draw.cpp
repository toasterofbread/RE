#include "draw.h"

#include "node/types/camera_3d.h"

#define DRAW_2D(code, screen_position) \
if (!screen_position && current_draw_mode != DRAW_MODE::TWO) { \
    if (Camera2D* camera = Engine::getSingleton()->getTree()->getEnabledCamera2D()) { \
        if (current_draw_mode == DRAW_MODE::THREE) { \
            EndMode3D(); \
        } \
        BeginMode2D(*camera->getCamera()); \
        current_draw_mode = DRAW_MODE::TWO; \
    } \
} \
else if (screen_position && current_draw_mode == DRAW_MODE::TWO) { \
    EndMode2D(); \
    current_draw_mode = DRAW_MODE::NONE; \
} \
code;

#define DRAW_3D(code) \
if (current_draw_mode != DRAW_MODE::THREE) { \
    if (Camera3D* camera = Engine::getSingleton()->getTree()->getEnabledCamera3D()) { \
        if (current_draw_mode == DRAW_MODE::TWO) { \
            EndMode2D(); \
        } \
        BeginMode3D(*camera->getCamera()); \
        current_draw_mode = DRAW_MODE::THREE; \
    } \
} \
code;

enum class DRAW_MODE {
    TWO, THREE, NONE
};
DRAW_MODE current_draw_mode = DRAW_MODE::NONE;

string load_message = "";
int load_count = 0;
const float LOAD_MARGIN = 0.1f;
const Colour LOAD_COLOUR = Colour::GREEN();

Texture2D getTexture(TEXTURE_TYPE texture) {
    #if PLATFORM == PLATFORM_VITA
    return texture->texture;
    #else
    return texture;
    #endif
}

void Draw::beginDrawing() {
    BeginDrawing();
    ClearBackground(Colour{100, 100, 100, 255});
}

void Draw::endDrawing() {
    if (current_draw_mode == DRAW_MODE::TWO) {
        EndMode2D();
    }
    else if (current_draw_mode == DRAW_MODE::THREE) {
        EndMode3D();
    }
    current_draw_mode = DRAW_MODE::NONE;

    EndDrawing();
}

void Draw::loadStep(string message, bool reverse) {

    Draw::beginDrawing();

    if (load_message != message) {
        load_message = message;
        load_count = reverse ? 542 : 0;
    }

    load_count += reverse ? -1 : 1;

    float w = OS::getScreenWidth();
    float h = OS::getScreenHeight();
    float progress = ((float)load_count) / 542.0f;

    drawLine(Vector2(w * LOAD_MARGIN, h / 2.0f), Vector2((w - (w * LOAD_MARGIN * 2.0f)) * progress + (w * LOAD_MARGIN), h / 2.0f), LOAD_COLOUR, true);
    drawText(message + " " + to_string(int(progress * 100.0f)) + "%", Vector2(w * LOAD_MARGIN, h / 2.0f - 20.0f), LOAD_COLOUR, 1.0f, true);

    Draw::endDrawing();

}

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, bool screen_position) {
    drawLine(Vector2(start_x, start_y), Vector2(end_x, end_y), colour, screen_position);
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour, bool screen_position) {
    DRAW_2D(DrawLine(start.x, start.y, end.x, end.y, colour), screen_position);
}

void Draw::drawTextureRST(TEXTURE_TYPE texture, Vector2 position, float rotation, Vector2 scale, Colour tint, bool screen_position) {
    DRAW_2D(DrawTexturePro(
        getTexture(texture),
        Rectangle{
            0,
            0,
            (float)OS::getTextureWidth(texture),// * (flip_x ? -1.0f : 1.0f),
            (float)OS::getTextureHeight(texture)// * (flip_y ? -1.0f : 1.0f)
        },
        Rectangle{
            position.x,
            position.y,
            (float)OS::getTextureWidth(texture) * scale.x,
            (float)OS::getTextureHeight(texture) * scale.y
        },
        Vector2::ZERO(),
        RAD2DEG(rotation),
        tint
    ), screen_position);
}

void Draw::drawText(string text, float pos_x, float pos_y, Colour colour, float size, bool screen_position) {
    drawText(text, Vector2(pos_x, pos_y), colour, size, screen_position);
}
void Draw::drawText(string text, Vector2 position, Colour colour, float size, bool screen_position) {
    DRAW_2D(DrawTextPro(GetFontDefault(), text.c_str(), position, Vector2::ZERO(), 0.0f, size * 10.0f, 1.0f, colour), screen_position);
}

void Draw::drawCube(Vector3 position, Vector3 size, Colour colour) {
    DRAW_3D(DrawCubeV(position, size, colour));
    DRAW_3D(DrawCubeWiresV(position, size, Colour::WHITE()));
}

void Draw::drawCube(Vector3 position, Vector3 size, TEXTURE_TYPE texture, Colour colour) {
    DRAW_3D(DrawCubeTexture(getTexture(texture), position, size.x, size.y, size.z, colour));
}

void Draw::drawGrid(int slices, float spacing) {
    DRAW_3D(DrawGrid(slices, spacing));
}
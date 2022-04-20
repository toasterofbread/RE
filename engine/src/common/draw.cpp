#include "draw.h"

string load_message = "";
int load_count = 0;
const float LOAD_MARGIN = 0.1f;
const Colour LOAD_COLOUR = Colour::GREEN();

void Draw::loadStep(string message, bool reverse) {

    OS::beginDrawing();

    if (load_message != message) {
        load_message = message;
        load_count = reverse ? 542 : 0;
    }

    load_count += reverse ? -1 : 1;

    float w = OS::getScreenWidth();
    float h = OS::getScreenHeight();
    float progress = ((float)load_count) / 542.0f;

    drawLine(Vector2(w * LOAD_MARGIN, h / 2.0f), Vector2((w - (w * LOAD_MARGIN * 2.0f)) * progress + (w * LOAD_MARGIN), h / 2.0f), LOAD_COLOUR, Draw::DRAW_MODE::SCREEN);
    drawText(message + " " + to_string(int(progress * 100.0f)) + "%", Vector2(w * LOAD_MARGIN, h / 2.0f - 20.0f), LOAD_COLOUR, 1.0f, Draw::DRAW_MODE::SCREEN);

    OS::endDrawing();

}

void Draw::beginDrawMode(DRAW_MODE mode) {
    if (mode == DRAW_MODE::WORLD) {
        if (Camera2D* camera = Engine::getSingleton()->getTree()->getEnabledCamera2D()) {
            BeginMode2D(camera->getCamera());
            return;
        }
    }
    EndMode2D();
}

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, DRAW_MODE mode) {
    drawLine(Vector2(start_x, start_y), Vector2(end_x, end_y), colour, mode);
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour, DRAW_MODE mode) {

    beginDrawMode(mode);

    // if (Camera2D* camera = getCamera(mode)) {
    //     Vector2 center = camera->getCenter();
    //     float rotation = camera->getDrawRotation();

    //     start.rotateAround(rotation, center);
    //     end.rotateAround(rotation, center);

    //     Vector2 offset = camera->getDrawOffset();

    //     float zoom = camera->getDrawZoom();
    //     Vector2 screen_size = OS::getScreenSize() / 2.0f;
    //     start = (start + offset - screen_size) * zoom + screen_size;
    //     end = (end + offset - screen_size) * zoom + screen_size;

    //     DrawLine(start.x, start.y, end.x, end.y, colour);
    // }
    // else {
    DrawLine(start.x, start.y, end.x, end.y, colour);
    // }

}

void Draw::drawTextureRST(TEXTURE_TYPE texture, Vector2 position, float rotation, Vector2 scale, Colour tint, DRAW_MODE mode) {

    beginDrawMode(mode);

    // if (Camera2D* camera = getCamera(mode)) {
    //     float zoom = camera->getDrawZoom();

    //     float draw_rotation = camera->getDrawRotation();
    //     position.rotateAround(draw_rotation, camera->getCenter());
    //     position += camera->getDrawOffset();

    //     Vector2 screen_size = OS::getScreenSize() / 2.0f;
    //     position = (position - screen_size) * zoom + screen_size;

    //     DrawTexturePro(
    //         texture,
    //         Rectangle{
    //             0,
    //             0,
    //             (float)OS::getTextureWidth(texture),// * (flip_x ? -1.0f : 1.0f),
    //             (float)OS::getTextureHeight(texture)// * (flip_y ? -1.0f : 1.0f)
    //         },
    //         Rectangle{
    //             position.x,
    //             position.y,
    //             (float)OS::getTextureWidth(texture) * scale.x * zoom,
    //             (float)OS::getTextureHeight(texture) * scale.y * zoom
    //         },
    //         Vector2::ZERO(),
    //         RAD2DEG(rotation + draw_rotation),
    //         tint
    //     );

    // }
    // else {
    DrawTexturePro(
        texture,
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
    );
    // }
}

void Draw::drawText(string text, float pos_x, float pos_y, Colour colour, float size, DRAW_MODE mode) {
    drawText(text, Vector2(pos_x, pos_y), colour, size, mode);
}
void Draw::drawText(string text, Vector2 position, Colour colour, float size, DRAW_MODE mode) {

    beginDrawMode(mode);

    // if (Camera2D* camera = getCamera(mode)) {
    //     float rotation = camera->getDrawRotation();

    //     position.rotateAround(rotation, camera->getCenter());
    //     position += camera->getDrawOffset();

    //     float zoom = camera->getDrawZoom();
    //     Vector2 screen_size = OS::getScreenSize() / 2.0f;
    //     position = (position - screen_size) * zoom + screen_size;

    //     DrawTextPro(GetFontDefault(), text.c_str(), position, Vector2::ZERO(), RAD2DEG(rotation), size * 10.0f * zoom, 1.0f, colour);
    // }
    // else {
    DrawTextPro(GetFontDefault(), text.c_str(), position, Vector2::ZERO(), 0.0f, size * 10.0f, 1.0f, colour);
    // }
}
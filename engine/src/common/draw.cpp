#include "draw.h"

string load_message = "";
int load_count = 0;
const float LOAD_MARGIN = 0.1f;
const Colour LOAD_COLOUR = GREEN;

void Draw::loadStep(string message) {

    OS::beginDrawing();

    if (load_message != message) {
        load_message = message;
        load_count = 0;
    }

    load_count++;

    float w = OS::getScreenWidth();
    float h = OS::getScreenHeight();
    float progress = ((float)load_count) / 542.0f;

    drawLine(Vector2(w * LOAD_MARGIN, h / 2.0f), Vector2((w - (w * LOAD_MARGIN * 2.0f)) * progress + (w * LOAD_MARGIN), h / 2.0f), LOAD_COLOUR, Draw::DRAW_MODE::SCREEN);
    drawText(message + " " + to_string(int(progress * 100.0f)) + "%", Vector2(w * LOAD_MARGIN, h / 2.0f - 20.0f), LOAD_COLOUR, 1.0f, Draw::DRAW_MODE::SCREEN);

    OS::endDrawing();

}

Vector2 Draw::getDrawOffset(DRAW_MODE mode) {
    if (mode == DRAW_MODE::WORLD) {
        Camera* camera = Engine::getSingleton()->getTree()->getEnabledCamera();
        if (camera) {
            return camera->getGlobalOffset();
        }
    }
    return VECTOR2ZERO;
}

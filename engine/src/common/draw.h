#ifndef INCLUDED_DRAW
#define INCLUDED_DRAW

#include "common/vector2.h"
#include "common/colour.h"
#include "node/types/camera_2d.h"

#include <string>
using namespace std;

// Forward declarations
class Camera2D;

namespace Draw {

    enum class DRAW_MODE {
        WORLD, SCREEN
    };

    void init();

    void beginDrawMode(DRAW_MODE mode);

    void loadStep(string message, bool reverse = false);

    // - Draw methods -

    void drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, DRAW_MODE mode = DRAW_MODE::WORLD);
    void drawLine(Vector2 start, Vector2 end, Colour colour, DRAW_MODE mode = DRAW_MODE::WORLD);

    void drawTexture(TEXTURE_TYPE texture, Vector2 position, DRAW_MODE mode = DRAW_MODE::WORLD);
    void drawTextureRST(TEXTURE_TYPE texture, Vector2 position, float rotation, Vector2 scale, Colour tint, DRAW_MODE mode = DRAW_MODE::WORLD);

    void drawText(string text, float pos_x, float pos_y, Colour colour, float size = 1.0f, DRAW_MODE mode = DRAW_MODE::SCREEN);
    void drawText(string text, Vector2 position, Colour colour, float size = 1.0f, DRAW_MODE mode = DRAW_MODE::SCREEN);

}

#endif
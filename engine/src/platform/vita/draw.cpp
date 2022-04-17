#include "platform/vita/defs.h"
#include "common/draw.h"

#include <vita2d.h>
#include "common/utils.h"
#include "common/colour.h"
#include "common/vector2.h"
#include "core/os.h"

#include "engine.h"

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, DRAW_MODE mode) {
    Vector2 offset = getDrawOffset(mode);
    vita2d_draw_line(start_x + offset.x, start_y + offset.y, end_x + offset.x, end_y + offset.y, colour.toInt());
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour, DRAW_MODE mode) {
    drawLine(start.x, start.y, end.x, end.y, colour, mode);
}

void Draw::drawTextureRST(VitaTexture* texture, Vector2 position, float rotation, Vector2 scale, Colour tint, DRAW_MODE mode) {
    VitaTextureData* data = texture->data;

    Vector2 offset = Vector2(data->w * scale.x * 0.5f, data->h * scale.y * 0.5f);
    offset.rotate(rotation);
    offset += getDrawOffset(mode);

    vita2d_draw_texture_part_tint_scale_rotate(texture->texture, position.x + offset.x, position.y + offset.y, data->x, data->y, data->w, data->h, scale.x, scale.y, rotation, tint.toInt());
}

static vita2d_pgf* font = NULL;
void Draw::drawText(string text, float pos_x, float pos_y, Colour colour, float size, DRAW_MODE mode) {
    if (font == NULL) {
        font = vita2d_load_default_pgf();
    }
    Vector2 offset = getDrawOffset(mode);
    vita2d_pgf_draw_text(font, pos_x + offset.x, pos_y + offset.y, colour.toInt(), size, text.c_str());
}
void Draw::drawText(string text, Vector2 position, Colour colour, float size, DRAW_MODE mode) {
    drawText(text, position.x, position.y, colour, size);
}
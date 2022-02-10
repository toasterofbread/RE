#include "animated_sprite.h"
#include "utils.h"

void AnimatedSprite::setSpriteSheet(SpriteSheet* value) {
    sprite_sheet = value;
}

void AnimatedSprite::process(float delta) {

    if (sprite_sheet == NULL) {
        return;
    }

    frame_timer += delta;

    if (frame_timer >= 1.0f/(float)frame_rate) {
        frame_timer = 0.0f;
        current_frame += 1;
    }

    SpriteSheet& sheet = *sprite_sheet;
    current_frame = current_frame % sheet.getFrameCount();

    DrawTexturePro(
        sheet.getTexture(),
        Rectangle{
            sheet.getFrameWidth() * current_frame,
            0, 
            sheet.getFrameWidth(),
            (float)sheet.getTexture().height
        },
        Rectangle{
            getGlobalPosition().x,
            getGlobalPosition().y, 
            sheet.getFrameWidth() * getGlobalScale().x,
            (float)sheet.getTexture().height * getGlobalScale().y
        },
        Vector2{0, 0},
        0.0,
        RAYWHITE
    );
}

// --- SpriteSheet ---

void SpriteSheet::setTexture(Texture2D sheet_texture, int frame_amount) {
    texture = sheet_texture;
    frame_count = frame_amount;
    frame_width = (float)(texture.width / frame_count);
}

int SpriteSheet::setTextureFile(const char *file_path, int frame_amount) {
    texture = LoadTexture(file_path);
    frame_count = frame_amount;
    frame_width = (float)(texture.width / frame_count);
    return 1;
}

Vector2 SpriteSheet::getFramePosition(int frame_idx) {
    return Vector2{frame_width * frame_idx, 0};
}
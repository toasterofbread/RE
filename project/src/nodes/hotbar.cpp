#include "hotbar.h"

void Hotbar::ready() {
    int filled = 0;
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (i < Block::getTypeCount() && Block::getTypeData((Block::TYPE)i).tangible)
            available_blocks[filled++] = (Block::TYPE)i;
    }

    for (; filled < HOTBAR_SIZE; filled++) {
        available_blocks[filled] = Block::TYPE::INVALID;
    }

    setScale(Vector2(1.25f, 1.25f));
    updatePosition();

    INPUT_EVENT_SELECT_LEFT.SIGNAL_TRIGGERED.connect(this, &Hotbar::selectLeft);
    INPUT_EVENT_SELECT_RIGHT.SIGNAL_TRIGGERED.connect(this, &Hotbar::selectRight);

    Engine::getSingleton()->SIGNAL_SCREEN_SIZE_CHANGED.connect(this, &Hotbar::updatePosition);
}

void Hotbar::draw() {
    Vector2 position = getPosition();
    Vector2 scale = getScale();

    for (int i = 0; i < HOTBAR_SIZE; i++) {
        Draw::drawTextureRST(grid_texture->getTexture(), position, 0.0f, scale, Colour(1, 1, 1, HOTBAR_OPACITY), true);
        
        if (available_blocks[i] != Block::TYPE::INVALID) {
            Texture2D& icon = Block::getTypeData(available_blocks[i]).icon;
            Draw::drawTextureRST(icon, 
                position + grid_texture->getSize() * scale / 2.0f - ((Vector2(icon.width, icon.height) / 2.0f) * scale * HOTBAR_ICON_SCALE),
                0.0f, scale * HOTBAR_ICON_SCALE, Colour::WHITE(), true);
        
            if (i == selected_item) {
                #define FONT_SIZE 2.0f
                Vector2 text_pos = Vector2(OS::getScreenWidth() / 2.0f, getPosition().y - 25);
                text_pos.x -= MeasureText(Block::getTypeName(available_blocks[i]).c_str(), FONT_SIZE) / 2.0f;
                Draw::drawText(Block::getTypeName(available_blocks[i]), text_pos, Colour::WHITE(), FONT_SIZE);
            }
        }
        
        if (i == selected_item)
            Draw::drawTextureRST(selection_texture->getTexture(), position, 0.0f, scale, Colour::WHITE(), true);
        
        position.x += grid_texture->getWidth() * scale.x;
    }
}

void Hotbar::selectLeft() {
    if (selected_item <= 0)
        selected_item = HOTBAR_SIZE - 1;
    else
        selected_item--;
}

void Hotbar::selectRight() {
    if (selected_item >= HOTBAR_SIZE - 1)
        selected_item = 0;
    else
        selected_item++;
}

void Hotbar::updatePosition() {
    setPosition(Vector2(
        OS::getScreenWidth() / 2.0f - (HOTBAR_SIZE * getScale().x / 2.0f) * grid_texture->getWidth(), OS::getScreenHeight() - grid_texture->getHeight() - HOTBAR_BOTTOM_MARGIN * getScale().y
    ));
}

Block::TYPE Hotbar::getSelected() {
    return available_blocks[selected_item];
}

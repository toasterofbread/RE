#include "sprite.h"

#include <memory>

#include "engine/src/engine_texture.h"

void Sprite::draw() {

    super::draw();

    if (!isGlobalVisible() || !hasTexture()) {
        return;
    }

    Vector2 origin;
    if (getRotateAroundCenter()) {
        origin = (texture->getSize() * getGlobalScale()) / 2.0f;
    }
    else {
        origin = getRotationOrigin();
    }

    // TODO origin
    Draw::drawTextureRST(texture->getTexture(), getGlobalPosition() + origin, getGlobalRotation(), getGlobalScale(), getGlobalModulate());

}

#include "sprite.h"

#include <memory>

#include "engine/src/engine_texture.h"

void Sprite::draw() {

    Node2D::draw();

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

    Vector2 global_position = getGlobalPosition() + origin;

    DrawTexturePro(
        texture->getTexture(),
        Rectangle{
            0,
            0,
            (float)texture->getWidth() * (flip_x ? -1.0f : 1.0f),
            (float)texture->getHeight() * (flip_y ? -1.0f : 1.0f)
        },
        Rectangle{
            global_position.x,
            global_position.y, 
            (float)texture->getWidth() * getGlobalScale().x,
            (float)texture->getHeight() * getGlobalScale().y
        },
        origin,
        getGlobalRotation() * RAD2DEG,
        getGlobalModulate()
    );

}

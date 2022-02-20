#include "animated_sprite.h"
#include "json.hpp"
using json = nlohmann::json;

#include "engine/src/utils.h"
#include "engine/src/core/resource/node_texture.h"
#include "engine/src/core/resource/sprite_animation.h"

void AnimatedSprite::process(float delta) {

    Node::process(delta);
    if (getPlaying()) frame_timer += delta;

    if (getAnimationSet() == NULL) {
        return;
    }
    if (!hasAnimation(getCurrentAnimationKey())) {
        if (!getCurrentAnimationKey().empty()) {
            warn("AnimatedSprite's animation_set has no animation called: " + getCurrentAnimationKey());
        }
        return;
    }

    SpriteAnimation* animation = getCurrentAnimation();

    if (frame_timer >= 1.0f/(float)animation->getFramerate()) {
        frame_timer = 0.0f;
        current_frame += 1;
    }

    current_frame = current_frame % animation->getFrameCount();

    if (getVisible()) {
        NodeTexture* frame_texture = animation->getFrame(current_frame);

        Vector2 origin;
        if (getRotateAroundCenter()) {
            origin = Vector2{frame_texture->getWidth() / 2.0f, frame_texture->getHeight() / 2.0f};
        }
        else {
            origin = getRotationOrigin();
        }

        DrawTexturePro(
            frame_texture->getTexture(),
            Rectangle{
                0,
                0,
                animation->getFrameWidth() * (flip_x ? -1.0f : 1.0f),
                (float)frame_texture->getHeight() * (flip_y ? -1.0f : 1.0f)
            },
            Rectangle{
                getGlobalPosition().x,
                getGlobalPosition().y, 
                animation->getFrameWidth() * getGlobalScale().x,
                (float)frame_texture->getHeight() * getGlobalScale().y
            },
            origin,
            getGlobalRotation(),
            RAYWHITE
        );
    }
}

void AnimatedSprite::setAnimationSet(SpriteAnimationSet* value) {
    if (getAnimationSet() != NULL) {
        getAnimationSet()->unlinkNode(this);
    }

    animation_set = value;
    animation_set->linkNode(this);
}

void AnimatedSprite::play(string animation_key) {
    setPlaying(true);
    if (animation_key != "") {
        setCurrentAnimationKey(animation_key);
    }
}

bool AnimatedSprite::hasAnimation(string animation_key) {
    if (getAnimationSet() == NULL) {
        return false;
    }
    return getAnimationSet()->hasAnimation(animation_key);
}

SpriteAnimation* AnimatedSprite::getCurrentAnimation() {
    if (getAnimationSet() == NULL) {
        warn("AnimatedSprite has no animation_set");
        return NULL;
    }
    return getAnimationSet()->getAnimation(getCurrentAnimationKey());
}

void AnimatedSprite::setCurrentAnimationKey(string value) {
    current_animation_key = value;
    if (getAnimationSet() != NULL && !current_animation_key.empty() && !hasAnimation(current_animation_key)) {
        warn("AnimatedSprite's animation_set has no animation called: " + current_animation_key);
    }
}
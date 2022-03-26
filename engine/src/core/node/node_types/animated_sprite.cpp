#include "animated_sprite.h"

#include <memory>
#include <json.hpp>
using json = nlohmann::json;

#include "engine/src/utils.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/resource/sprite_animation.h"

void AnimatedSprite::process(float delta) {
    Sprite::process(delta);

    if (getPlaying()) frame_timer += delta;

    if (getAnimationSet() == NULL) {
        return;
    }
    if (!hasAnimation(getCurrentAnimationKeys())) {
        if (!getCurrentAnimationKeys().empty()) {
            warn("AnimatedSprite's animation_set has no animation at key [" + strVector2str(getCurrentAnimationKeys()) + "]");
        }
        return;
    }

    shared_ptr<SpriteAnimation> animation = getCurrentAnimation();

    if (frame_timer >= 1.0f/(float)animation->getFramerate()) {
        frame_timer = 0.0f;
        current_frame += 1;

        if (current_frame >= animation->getFrameCount()) {
            if (animation->getLoop()) {
                current_frame = 0;
                SIGNAL_ANIMATION_ENDED->emit(true);
            }
            else {
                current_frame = animation->getFrameCount() - 1;
                SIGNAL_ANIMATION_ENDED->emit(false);
            }
        }
    }

    setTexture(animation->getFrame(current_frame));

    // current_frame = current_frame % animation->getFrameCount();
}

void AnimatedSprite::setAnimationSet(shared_ptr<SpriteAnimationSet> value) {
    animation_set = value;
}

void AnimatedSprite::play(string animation_key, bool reset_frame) {
    setPlaying(true);
    if (vector<string>{animation_key} != getCurrentAnimationKeys()) {
        if (reset_frame) setCurrentFrame(0);
        setCurrentAnimationKey(animation_key);
    }
}

void AnimatedSprite::play(vector<string> animation_keys, bool reset_frame) {
    setPlaying(true);
    if (animation_keys != getCurrentAnimationKeys()) {
        if (reset_frame) setCurrentFrame(0);
        setCurrentAnimationKeys(animation_keys);
    }
}

bool AnimatedSprite::hasAnimation(string animation_key) {
    if (getAnimationSet() == NULL) {
        return false;
    }
    return getAnimationSet()->hasAnimation(animation_key);
}

bool AnimatedSprite::hasAnimation(vector<string> animation_keys) {
    if (getAnimationSet() == NULL) {
        return false;
    }
    return getAnimationSet()->hasAnimation(animation_keys);
}

shared_ptr<SpriteAnimation> AnimatedSprite::getCurrentAnimation() {
    if (getAnimationSet() == NULL) {
        warn("AnimatedSprite has no animation_set");
        return NULL;
    }
    return getAnimationSet()->getAnimation(getCurrentAnimationKeys());
}

void AnimatedSprite::setCurrentAnimationKey(string value) {
    current_animation_keys.clear();
    current_animation_keys.push_back(value);
}

void AnimatedSprite::setCurrentAnimationKeys(vector<string> value) {
    current_animation_keys = value;
}

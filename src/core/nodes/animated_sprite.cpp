#include "animated_sprite.h"
#include "utils.h"
#include "json.hpp"
using json = nlohmann::json;

void AnimatedSprite::process(float delta) {

    Node::process(delta);

    if (animation_set == NULL) {
        return;
    }
    if (!getPlaying()) {
        return;
    }

    // SpriteAnimationSet set = *getAnimationSet();
    // std::unordered_map<const char*, SpriteAnimation*> anims = set.getAnimations();

    // for (std::unordered_map<const char*, SpriteAnimation*>::iterator it=anims.begin(); it!=anims.end(); ++it)
    //     std::cout << it->first << " => " << (*it->second).getName() << '\n';

    if (!hasAnimation(getCurrentAnimationKey())) {
        warn(concatenateChars("AnimatedSprite's animation_set has no animation called: ", getCurrentAnimationKey()));
        return;
    }

    frame_timer += delta;

    if (frame_timer >= 1.0f/(float)frame_rate) {
        frame_timer = 0.0f;
        current_frame += 1;
    }

    IC(getCurrentAnimationKey());
    IC(getCurrentAnimation() == NULL);

    SpriteAnimation* animation = getCurrentAnimation();
    current_frame = current_frame % animation->getFrameCount();

    Texture2D& frame_texture = *animation->getFrame(current_frame);

    DrawTexturePro(
        frame_texture,
        Rectangle{
            (float)animation->getFrameWidth() * current_frame,
            0,
            animation->getFrameWidth() * (flip_x ? 1.0f : -1.0f),
            (float)frame_texture.height * (flip_y ? -1.0f : 1.0f)
        },
        Rectangle{
            getGlobalPosition().x,
            getGlobalPosition().y, 
            animation->getFrameWidth() * getGlobalScale().x,
            (float)frame_texture.height * getGlobalScale().y
        },
        Vector2{0, 0},
        0.0,
        RAYWHITE
    );
}

void AnimatedSprite::setAnimationSet(SpriteAnimationSet* value) {
    animation_set = value;

    std::unordered_map<const char*, SpriteAnimation*> animations = animation_set->getAnimations();

    // for (std::unordered_map<const char*, SpriteAnimation*>::iterator it=animations.begin(); it!=animations.end(); ++it) {
    //     const char* key = it->first; 
    //     SpriteAnimation* = it->second;

    // }

    for ( auto it = animations.begin(); it != animations.end(); ++it ) {
        const char* key = it->first; 
        SpriteAnimation* animation = it->second;
        print(animation->getName());
    }
}

void AnimatedSprite::play(const char* animation_key) {
    setPlaying(true);
    if (animation_key != "") {
        setCurrentAnimationKey(animation_key);
    }
}

bool AnimatedSprite::hasAnimation(const char* animation_key) {
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

void AnimatedSprite::setCurrentAnimationKey(const char* value) {
    current_animation_key = value;
    if (getAnimationSet() != NULL && !hasAnimation(current_animation_key)) {
        warn(concatenateChars("AnimatedSprite's animation_set has no animation called: ", current_animation_key));
    }
}
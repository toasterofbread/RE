#ifndef INCLUDED_ANIMATED_SPRITE
#define INCLUDED_ANIMATED_SPRITE

#include <iostream>
#include <memory>

#include "engine/src/node/types/sprite.h"

// Forward declarations
class AnimatedSprite;
class SpriteAnimation;
class SpriteAnimationSet;
class Engine;

class AnimatedSprite: public Sprite {
    private:
        shared_ptr<SpriteAnimationSet> animation_set = NULL;
        vector<string> current_animation_keys;

        int current_frame = 0;
        float frame_timer = 0.0f;

        bool playing = false;

        void process(float delta);

    public:

        REGISTER_NODE(AnimatedSprite, Sprite, {
            c->template registerProperty<shared_ptr<SpriteAnimationSet>>("animation_set", &NodeType::setAnimationSet)
            ->template registerProperty<string>("current_animation_key", &NodeType::setCurrentAnimationKey)
            ->template registerProperty<int>("current_frame", &NodeType::setCurrentFrame)
            ->template registerProperty<bool>("playing", &NodeType::setPlaying);
        });
                
        Signal<bool> SIGNAL_ANIMATION_ENDED;

        void play(string animation_key = "", bool reset_frame = true);
        void play(vector<string> animation_keys, bool reset_frame = true);
        
        bool hasAnimation(string animation_key);
        bool hasAnimation(vector<string> animation_keys);
        
        shared_ptr<SpriteAnimation> getCurrentAnimation();

        void setAnimationSet(shared_ptr<SpriteAnimationSet> value);
        shared_ptr<SpriteAnimationSet> getAnimationSet() { return animation_set; }
        
        void setCurrentAnimationKey(string value);
        void setCurrentAnimationKeys(vector<string> value);
        vector<string> getCurrentAnimationKeys() { return current_animation_keys; }
        
        void setCurrentFrame(int value) { current_frame = value; }
        int getCurrentFrame() { return current_frame; }

        void setPlaying(bool value) {playing = value;}
        bool getPlaying() {return playing;}
    
};

#endif
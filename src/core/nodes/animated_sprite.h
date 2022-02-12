#include "raylib-cpp.hpp"
#include "core/node.h"
#include "core/resources/sprite_animation.h"
#include "json.hpp"
using json = nlohmann::json;

class AnimatedSprite: public Node {
    private:
        SpriteAnimationSet* animation_set = NULL;
        const char* current_animation_key = "";

        int current_frame = 0;
        int frame_rate = 1;
        float frame_timer = 0.0f;

        bool playing = false;
        bool flip_x = false;
        bool flip_y = false;

        void process(float delta);
    public:
        virtual const char* getTypeName() {return "AnimatedSprite";}
        
        void play(const char* animation_key = "");
        bool hasAnimation(const char* animation_key);
        SpriteAnimation* getCurrentAnimation();

        void setAnimationSet(SpriteAnimationSet* value);
        SpriteAnimationSet* getAnimationSet() { return animation_set; }
        void setCurrentAnimationKey(const char* value);
        const char* getCurrentAnimationKey() { return current_animation_key; }
        void setPlaying(bool value) {playing = value;}
        bool getPlaying() {return playing;}
        void setFlipX(bool value) {flip_x = value;}
        bool getFlipX() {return flip_x;}
        void setFlipY(bool value) {flip_y = value;}
        bool getFlipY() {return flip_y;}

        void setFrameRate(int value) {frame_rate = value;}
        int getFrameRate() {return frame_rate;}

        
};

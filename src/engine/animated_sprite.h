#include "raylib-cpp.hpp"
#include "node.h"

class SpriteSheet {
    private:
        Texture2D texture;
        float frame_width;
        int frame_count;
    public:
        void setTexture(Texture2D texture, int frame_count);
        int setTextureFile(const char *file_path, int frame_count);
        Vector2 getFramePosition(int frame_idx);

        int getFrameCount() {return frame_count;}
        float getFrameWidth() {return frame_width;}
        Texture2D getTexture() {return texture;}
};

class AnimatedSprite: public Node {
    private:
        SpriteSheet* sprite_sheet = NULL;

        int current_frame = 0;
        int frame_rate = 1;
        float frame_timer = 0.0f;

        bool playing = false;

        void process(float delta);
    public:
        void setSpriteSheet(SpriteSheet* value);
        
        void setPlaying(bool value) {playing = value;}
        bool getPlaying() {return playing;}

        void setFrameRate(int value) {frame_rate = value;}
        int getFrameRate() {return frame_rate;}

        
};

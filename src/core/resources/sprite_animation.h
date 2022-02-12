#include "raylib-cpp.hpp"
#include "json.hpp"
#include "core/resource.h"
using json = nlohmann::json;

class SpriteAnimation {
    private:
        const char* name = "Unnamed SpriteAnimation";
        bool loop = false;
        int framerate = 30;
        std::vector<Texture2D*> frames;
    public:
        void init(const char* animation_name, json* animation_data, json* file_data, Node* host, const char* load_directory);

        const char* getName() { return name; }
        bool getLoop() { return loop; }
        int getFramerate() { return framerate; }
        std::vector<Texture2D*>* getFrames() { return &frames; }
        Texture2D* getFrame(int frame_idx);
        int getFrameCount();
        int getFrameWidth() { return frames[0]->width; }
};

class SpriteAnimationSet: public Resource {
    private:
        const char* file_path;
        json data;
        std::unordered_map<const char*, SpriteAnimation*> animations;
    public:
        SpriteAnimationSet(Node* host_node): Resource(host_node) {}
        void loadFile(const char* file_path, const char* load_directory = "/");
        bool hasAnimation(const char* animation_key);
        SpriteAnimation* getAnimation(const char* animation_key);
        Texture2D* getFrame(const char* animation_key, int frame_idx);
        std::unordered_map<const char*, SpriteAnimation*> getAnimations() {return animations;}

};
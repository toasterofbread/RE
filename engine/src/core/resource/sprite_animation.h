#include <raylib-cpp.hpp>
#include <memory>
#include <json.hpp>
using json = nlohmann::json;

#include "engine/src/core/resource/resource.h"

// Forward declarations
class EngineTexture;
class Engine;

class SpriteAnimation {
    private:
        string name = "Unnamed SpriteAnimation";
        bool loop = false;
        int framerate = 30;
        vector<shared_ptr<EngineTexture>> frames;
    public:
        void init(string animation_name, json* animation_data, json* file_data, Engine* engine, string load_directory);

        string getName() { return name; }
        bool getLoop() { return loop; }
        int getFramerate() { return framerate; }
        vector<shared_ptr<EngineTexture>> getFrames() { return frames; }
        shared_ptr<EngineTexture> getFrame(int frame_idx);
        
        int getFrameCount();
        bool hasFrame(int frame_idx);

        int getFrameWidth(int frame_idx = 0);
        int getFrameHeight(int frame_idx = 0);
        Vector2 getFrameSize(int frame_idx = 0);
};

class SpriteAnimationSet: public Resource {
    public:
        SpriteAnimationSet(Engine* engine, string file_path, string base_directory_override = "//");

        bool hasAnimation(string animation_key);
        SpriteAnimation* getAnimation(string animation_key);
        shared_ptr<EngineTexture> getFrame(string animation_key, int frame_idx);
        unordered_map<string, SpriteAnimation*> getAnimations() {return animations;}

        static bool isFileValid(string file_path, string base_directory_override = "//", string* error_container = NULL);
    private:
        string file_path;
        json data;
        unordered_map<string, SpriteAnimation*> animations;

        void loadFile(Engine* engine, string file_path, string base_directory_override = "//");
};
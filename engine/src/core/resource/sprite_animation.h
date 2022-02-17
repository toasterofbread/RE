#include "raylib-cpp.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include "engine/src/core/resource/resource.h"

// Forward declarations
class NodeTexture;

class SpriteAnimation {
    private:
        std::string name = "Unnamed SpriteAnimation";
        bool loop = false;
        int framerate = 30;
        std::vector<NodeTexture*> frames;
    public:
        void init(std::string animation_name, json* animation_data, json* file_data, Node* initial_linked_node, std::string load_directory);

        std::string getName() { return name; }
        bool getLoop() { return loop; }
        int getFramerate() { return framerate; }
        std::vector<NodeTexture*> getFrames() { return frames; }
        NodeTexture* getFrame(int frame_idx);
        int getFrameCount();
        int getFrameWidth();
};

class SpriteAnimationSet: public Resource {
    public:
        SpriteAnimationSet(Node* initial_linked_node, string file_path, string base_directory_override = "//");
        bool hasAnimation(string animation_key);
        SpriteAnimation* getAnimation(string animation_key);
        NodeTexture* getFrame(string animation_key, int frame_idx);
        std::unordered_map<string, SpriteAnimation*> getAnimations() {return animations;}

        static bool isFileValid(string file_path, string base_directory_override = "//", string* error_container = NULL);
    private:
        std::string file_path;
        json data;
        std::unordered_map<std::string, SpriteAnimation*> animations;

        void loadFile(Node* initial_linked_node, string file_path, string base_directory_override = "//");
        
        void onNodeLinked(Node* node);
        void onNodeUnlinked(Node* node);
};
#include "raylib-cpp.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include "engine/src/core/resource/resource.h"

// Forward declarations
class NodeTexture;

class SpriteAnimation {
    private:
        string name = "Unnamed SpriteAnimation";
        bool loop = false;
        int framerate = 30;
        vector<NodeTexture*> frames;
    public:
        void init(string animation_name, json* animation_data, json* file_data, Node* initial_linked_node, string load_directory);

        string getName() { return name; }
        bool getLoop() { return loop; }
        int getFramerate() { return framerate; }
        vector<NodeTexture*> getFrames() { return frames; }
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
        unordered_map<string, SpriteAnimation*> getAnimations() {return animations;}

        static bool isFileValid(string file_path, string base_directory_override = "//", string* error_container = NULL);
    private:
        string file_path;
        json data;
        unordered_map<string, SpriteAnimation*> animations;

        void loadFile(Node* initial_linked_node, string file_path, string base_directory_override = "//");
        
        void onNodeLinked(Node* node);
        void onNodeUnlinked(Node* node);

        bool initialised = false;
};
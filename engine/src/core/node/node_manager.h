#include <yaml-cpp/yaml.h>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>
#include <unordered_map>
#include <string>
#include <thread>

#include "engine/src/core/node/node.h"
#include "engine/src/input/input.h"

// Forward declarations
class NodeTexture;
class AnimatedSprite;
class Resource;

class NodeManager {
    public:
        NodeManager();
        void ensureAsync();

        void rebuildAndRun();

        void process(float delta);
        void addNode(Node* node);
        Node* loadScene(string file_path, Node* root_node_override = NULL, YAML::Node* config_container = NULL);
        static void processSceneConfig(YAML::Node& config, Node* root_node);

        Node* getRoot() { return root_node; }
        Input* getInput() { return input_instance; }

        int getNewNodeId();
        
        Texture2D loadTexture(string file_path, NodeTexture* node_texture);
        void unloadTexture(Texture2D texture, NodeTexture* node_texture);
        bool isTextureLoaded(string file_path);
        // bool isNodeUsingTexture() // TODO

        void resourceCreated(Resource* resource);
        void resourceDeleted(Resource* resource);

        void inputEventCreated(InputEvent* event);
        void inputEvenDeleted(InputEvent* event);

        Node* synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override);

        template<typename T>
        void requestDeletion(T* what) { delete what; }

        void addMacro(function<void(void)> func, vector<Input::GAMEPAD_BUTTON> gamepad_button_combination, string label, bool display_label);
        void addMacro(function<void(void)> func, vector<Input::KEYBOARD_BUTTON> keyboard_button_combination, string label, bool display_label);
    
    private:
        int current_id_max = 0;
        Node* root_node = new Node(this);
        unordered_map<string, Texture2D> loaded_textures;
        unordered_map<unsigned int, vector<NodeTexture*>> texture_resources;

        Node* instanceNodeFromType(string& node_type, YAML::Node& node_data, bool suppress_warning = false, Node* root_node_override = NULL);

        vector<Resource*> all_resources;
        vector<InputEvent*> all_inputevents;
        thread::id main_thread_id = this_thread::get_id();

        Input* input_instance = new Input(this);
};
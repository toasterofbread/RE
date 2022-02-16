#include <yaml-cpp/yaml.h>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>
#include <unordered_map>
#include <string>

#include "engine/src/core/node/node.h"

// Forward declarations
class Node;
class NodeTexture;
class AnimatedSprite;
class Resource;

class NodeManager {
    private:
        int current_id_max = 0;
        Node* root_node = new Node(this);
        unordered_map<string, Texture2D> loaded_textures;
        unordered_map<unsigned int, vector<NodeTexture*>> texture_resources;

        Node* instanceNodeFromType(string& node_type, YAML::Node& node_data, bool suppress_warning = false, Node* root_node_override = NULL);

        vector<Resource*> all_resources;
    public:
        NodeManager();

        void processNodes(float delta);
        void addNode(Node* node);
        Node* loadScene(string file_path, Node* root_node_override = NULL, YAML::Node* config_container = NULL);
        static void processSceneConfig(YAML::Node& config, Node* root_node);

        Node* getRoot() {return root_node;}

        int getNewNodeId();
        
        Texture2D loadTexture(string file_path, NodeTexture* node_texture);
        void unloadTexture(Texture2D texture, NodeTexture* node_texture);
        bool isTextureLoaded(string file_path);
        // bool isNodeUsingTexture() // TODO

        void resourceCreated(Resource* resource);
        void resourceDeleted(Resource* resource);

        Node* synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override);

        template<typename T>
        void requestDeletion(T* what) { delete what; }
};
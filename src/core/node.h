#ifndef INCLUDED_NODE
#define INCLUDED_NODE

#include "raylib-cpp.hpp"
#include "iostream"
#include "vector"

#include "unordered_map"
#include "icecream.hpp"

using namespace std;

const int MAX_CHILDREN = 100;

class NodeManagerParent {
    private:
        int current_id_max = 0;
        unordered_map<const char*, Texture2D> loaded_textures;
        unordered_map<const char*, vector<int>> loaded_texture_users;
    public:        
        int getNewId();
        Texture2D* loadTexture(const char* file_path, int node_id);
        void unloadTexture(Texture2D* texture, int node_id);
        bool isTextureLoaded(const char* file_path);
        // bool isNodeUsingTexture() // TODO
};

class Node {
    private:
        bool initialised = false;

        vector<Node*> children;
        int id = -1;

        Node* parent = NULL;
        Node* root = NULL;
        bool is_root = false;
        NodeManagerParent* manager = NULL;

        bool show_gizmos = false;
        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};

    public:
        // - Core -
        void init(Node* root_node, Node* parent_node, NodeManagerParent* node_manager);
        virtual void process(float delta);
        virtual void ready();
        virtual const char* getTypeName() {return "Node";}

        // - Resources -
        Texture2D* loadTexture(const char* file_path);
        void unloadTexture(Texture2D* texture) { manager->unloadTexture(texture, getId()); }
        bool isTextureLoaded(const char* file_path) { return manager->isTextureLoaded(file_path); }

        // - Children-
        void addChild(Node* child);
        Node* getChild(int child_idx);
        vector<Node*> getChildren();
        int getChildCount();

        // - Hierarchy -
        bool isInsideTree();
        Node* getParent();
        Node* getRoot();
        NodeManagerParent* getManager();
        int getIndex(); // Index of this within parent's children
        int getId(); // Unique ID of this node
        void kill();

        // - Values -
        bool getShowGizmos() { return show_gizmos; }
        void setShowGizmos(bool value) { show_gizmos = value; }

        void setPosition(Vector2 value);
        Vector2 getPosition();
        void setGlobalPosition(Vector2 value);
        Vector2 getGlobalPosition(bool suppress_warning = false);

        void setScale(Vector2 value);
        Vector2 getScale();
        void setGlobalScale(Vector2 value);
        Vector2 getGlobalScale(bool suppress_warning = false);

};


#endif

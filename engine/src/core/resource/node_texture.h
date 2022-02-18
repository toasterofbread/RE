#include "raylib-cpp.hpp"
#include "engine/src/core/resource/resource.h"

class NodeTexture: public Resource {
    public:
        NodeTexture(Node* initial_linked_node, string autoload_file_path = "");
        
        void load(string file_path);
        void unload();

        bool isTextureLoaded() { return texture_loaded; }
        Texture2D getTexture();

        void setAutoload(string value) { autoload_path = value; }
        string getAutoload() { return autoload_path; }
        bool hasAutoload() { return !autoload_path.empty(); }

        // Texture functions
        int getWidth();
        int getHeight();

    private:
        Texture2D texture;
        bool texture_loaded = false;

        string autoload_path;

        void onNodeLinked(Node* node);
        void onNodeUnlinked(Node* node);
        void onDeleted();
};

void onNodeLinked(void* _self, Node* node);
void onNodeUnlinked(void* _self, Node* node);
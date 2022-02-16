#include "raylib-cpp.hpp"
#include "engine/src/core/resource/resource.h"

class NodeTexture: public Resource {
    public:
        NodeTexture(Node* initial_linked_node, std::string autoload_file_path = "");
        
        void load(std::string file_path);
        void unload();

        bool isTextureLoaded() { return texture_loaded; }
        Texture2D getTexture();

        void setAutoload(std::string value) { autoload_path = value; }
        std::string getAutoload() { return autoload_path; }
        bool hasAutoload() { return !autoload_path.empty(); }

        // Texture functions
        int getWidth();
        int getHeight();

    private:
        Texture2D texture;
        bool texture_loaded = false;

        std::string autoload_path;

        static void onNodeLinked(void* _self, Node* node);
        static void onNodeUnlinked(void* _self, Node* node);
        static void onDeleted(void* _self);
};

void onNodeLinked(void* _self, Node* node);
void onNodeUnlinked(void* _self, Node* node);
#ifndef INCLUDED_scene_tree
#define INCLUDED_scene_tree

#include <yaml-cpp/yaml.h>
#include <string>
using namespace std;

#include "common/utils.h"

#include <vector>
using namespace std;

// Forward declaration
class Engine;
class Node;
class Node2D;
class Timer;

class SceneTree {
    public:
        enum class STATE {
            PREINIT, IDLE, PROCESS, POST_PROCESS, DRAW
        };

        void init();

        void process(float delta);
        void addNode(Node* node);
        void queueNodeKill(Node* node);

        Node* getRoot() { return root_node; }

        STATE getCurrentState() { return current_state; }

        void onNodeAddedToTree(Node* node);
        void onNodeRemovedFromTree(Node* node);

        Timer* createTimer(float duration, bool free_on_timeout = true);

        static const int MIN_DRAW_LAYER = -2048;
        static const int MAX_DRAW_LAYER = 2048;

    private:
        Node* root_node;
        
        vector<Node2D*> drawable_nodes[MAX_DRAW_LAYER - MIN_DRAW_LAYER];

        STATE current_state = STATE::PREINIT;
        vector<Node*> kill_queue;

        void onDrawableNodeLayerChanged(int old_draw_layer, int new_draw_layer, Node2D* node);
};

#endif
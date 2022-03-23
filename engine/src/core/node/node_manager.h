#ifndef INCLUDED_NODE_MANAGER
#define INCLUDED_NODE_MANAGER

#include <yaml-cpp/yaml.h>
#include <string>
using namespace std;

#include "engine/src/utils.h"

#include "icecream.hpp" // Debug

// Forward declaration
class Engine;
class Node;

class SceneTree {
    public:
        enum class STATE {
            PREINIT, IDLE, PROCESS, POST_PROCESS
        };

        SceneTree(Engine* engine_singleton);
        void init();

        void process(float delta);
        void addNode(Node* node);
        void queueNodeKill(Node* node);

        void onNodeCreated(Node* node);
        void onNodeKilled(Node* node);
        int getGlobalNodeCount();

        Node* getRoot() { return root_node; }

        int getNewNodeId();
        STATE getCurrentState() { return current_state; }
       
        void objectRegisteredAsConstructor(string object_type_name, bool is_node) {
            if (is_node) {
                registered_nodes.push_back(object_type_name);
            }
        }

        bool isNodeTypeRegistered(string node_type_name) {
            return vectorContainsValue(&registered_nodes, node_type_name);
        }

    private:
        Engine* engine;
        Node* root_node;

        int current_id_max = 0;
        vector<string> registered_nodes;
        int global_node_count = 0;

        STATE current_state = STATE::PREINIT;
        vector<Node*> kill_queue;
};

#endif
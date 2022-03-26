#ifndef INCLUDED_NODE
#define INCLUDED_NODE

#include <raylib-cpp.hpp>
#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>
using namespace std;

#include "engine/src/engine.h"

#include <icecream.hpp> // Debug

// Forward declarations
class SceneTree;
template<typename A, typename... B>
class Signal;
template<typename ObjectType>
class ObjectConstructor;

class Node {
    private:
        vector<Node*> children{};
        unordered_map<string, Node*> registered_scene_nodes;
        int id = -1;

        Node* parent = NULL;
        bool is_root = false;
        SceneTree* tree = NULL;

    protected:
    
        virtual void addedToNode(Node* parent_node);
        virtual void removedFromNode(Node* former_parent_node);
        string name;
    public:
        Node();
        virtual ~Node() {
            delete SIGNAL_KILLED;
        }

        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_type_name);
        static string getTypeName() { return "Node"; }

        // - Signals -
        Signal<void, Node*>* SIGNAL_READY;
        Signal<void, Node*>* SIGNAL_KILLED;

        // - Core -
        virtual void ready();
        virtual void process(float delta);
        virtual void physicsProcess(float delta);
        string getValidName(string base_name = "");
        void makeRoot(SceneTree* of_tree);

        // - Children-
        void addChild(Node* child);

        void removeChild(int child_idx);
        void removeChild(string child_name);
        void removeChild(Node* child);

        Node* getChild(int child_idx);
        Node* getChild(string child_name);
        Node* getChildAtPath(string child_path);
        
        bool hasChild(int child_idx);
        bool hasChild(string child_name);
        bool hasChildAtPath(string child_path);
        
        vector<Node*>* getChildren();
        vector<pair<Node*, int>> getChildrenRecursive(int max_depth = -1, int depth_offset = 0);
        
        int getChildCount();
        int getChildIndex(Node* child); // Index of given child within children

        void registerSceneNode(string key, Node* node) { registered_scene_nodes[key] = node; }
        bool hasRegisteredSceneNode(string key) { return registered_scene_nodes.count(key); }
        Node* getResgisteredSceneNode(string key) { return registered_scene_nodes[key]; }

        // - Hierarchy -
        bool isInsideTree();
        bool isIndirectParentOf(Node* node);
        Node* getParent();
        bool isRoot();
        SceneTree* getTree();
        void printTree(int max_depth = -1);
        int getIndex(); // Index of this within parent's children
        int getId(); // Unique ID of this node
        void kill();
        void queue_kill();

        // - Values -
        void setName(string value);
        string getName() { return name; }

};

#endif

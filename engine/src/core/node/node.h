#ifndef INCLUDED_NODE
#define INCLUDED_NODE

#include "engine/src/raylib_include.h"
#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>
using namespace std;

#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

// Forward declarations
class SceneTree;
template<typename ObjectType>
class ObjectConstructor;

#define REGISTER_NODE_WITH_CONSTRUCTOR(name, constructor_code) \
static string getTypeNameStatic() {      \
    return #name;                        \
}                                        \
string getTypeName() {                   \
    return #name;                        \
}                                        \
name() {                                 \
    setName(getTypeName());              \
    constructor_code;                    \
}

#define REGISTER_NODE(name) REGISTER_NODE_WITH_CONSTRUCTOR(name, {})

class Node {
    private:
        vector<Node*> children{};
        unordered_map<string, Node*> registered_scene_nodes;
        int id = -1;
        bool ready_called = false;

        Node* parent = NULL;
        bool is_root = false;
        SceneTree* tree = NULL;

    protected:
        virtual void addedToNode(Node* parent_node);
        virtual void removedFromNode(Node* former_parent_node);
        string name;

    public:
        // DEBUG
        bool notify_when_killed = false;

        Node();

        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_type_name);
        static string getTypeNameStatic() { return "Node"; }
        virtual string getTypeName() { return "Node"; }

        // - Signals -
        Signal<> SIGNAL_READY;
        Signal<> SIGNAL_KILLED;

        // - Core -
        virtual void ready();
        virtual void process(float delta);
        virtual void physicsProcess(float delta);
        string getValidName(string base_name = "");
        void makeRoot(SceneTree* of_tree);

        // - Children-
        virtual void addChild(Node* child);

        void removeChild(int child_idx);
        void removeChild(string child_name);
        virtual void removeChild(Node* child);

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
        void queueKill();

        bool isReady() { return ready_called; }

        // - Values -
        void setName(string value);
        string getName() { return name; }

        // - Conversion -
        operator string() {
            return "Node type: " + getTypeName() + " | ID: " + int2char(getId()) + " | Name: " + getName();
        }

};

#endif

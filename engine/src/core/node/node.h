#ifndef INCLUDED_NODE
#define INCLUDED_NODE
using namespace std;

#include <raylib-cpp.hpp>
#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>

#include "engine/src/core/signal.h"

// Debug
#include <icecream.hpp>

// Forward declarations
class NodeTexture;
class NodeManager;
class Node;

PONDER_TYPE(Node)
class Node {
    private:
        // bool initialised = false;

        vector<Node*> children{};
        unordered_map<string, Node*> registered_scene_nodes;
        int id = -1;

        vector<Signal*> signals;
        Node* parent = NULL;
        bool is_root = false;
        NodeManager* manager = NULL;

        bool show_gizmos = false;
        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};
        float rotation = 0.0f;
        bool visible = true;

        void init(NodeManager* node_manager);
    protected:
        string name;
    public:
        Node(NodeManager* node_manager);

        // - Signals -
        NodeSignalStatic* SIGNAL_READY;
        NodeSignalStatic* SIGNAL_KILLED;

        // - Ponder -
        static string getSetterName(string property_name) {
            return "set " + property_name + " ";
        }

        template<typename NodeType>
        static ponder::ClassBuilder<NodeType> getNodeBuilder(string node_name) {
            if (isPonderClassDeclared(node_name)) return ponder::ClassBuilder<NodeType>((ponder::Class&)ponder::classByName(node_name));
            return ponder::Class::declare<NodeType>(node_name);
        }

        static bool isPonderClassDeclared(string class_name) {
            for (auto&& cls : ponder::classIterator()) {
                if (cls.first == class_name) return true;
            }
            return false;
        }

        template<typename NodeType>
        static ponder::ClassBuilder<NodeType> declareSetters(string node_name) {
            return getNodeBuilder<NodeType>(node_name)
                .function("set name string", &NodeType::setName)
                .function("set show_gizmos bool", &NodeType::setShowGizmos)
                .function("set position Vector2", &NodeType::setPosition)
                .function("set scale Vector2", &NodeType::setScale)
                .function("set rotation float", &NodeType::setRotation)
                .function("set visible bool", &NodeType::setVisible)
                ;
        }

        // - Core -
        void addedToNode(Node* parent_node);
        void removedFromNode(Node* former_parent_node);
        virtual void ready();
        virtual void process(float delta);
        virtual string getTypeName() {return "Node";}
        std::string getValidName(std::string base_name = "");

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
        Node* getParent();
        Node* getRoot();
        bool isRoot();
        NodeManager* getManager();
        void printTree(int max_depth = -1);
        int getIndex(); // Index of this within parent's children
        int getId(); // Unique ID of this node
        void kill();

        // - Values -
        void setShowGizmos(bool value) { show_gizmos = value; }
        bool getShowGizmos() { return show_gizmos; }

        void setName(string value);
        string getName() { return name; }

        void setPosition(Vector2 value);
        Vector2 getPosition();
        void setGlobalPosition(Vector2 value);
        Vector2 getGlobalPosition(bool suppress_warning = false);

        void setScale(Vector2 value);
        Vector2 getScale();
        void setGlobalScale(Vector2 value);
        Vector2 getGlobalScale(bool suppress_warning = false);

        void setRotation(float value);
        float getRotation();
        void setGlobalRotation(float value);
        float getGlobalRotation(bool suppress_warning = false);

        void setVisible(bool value) { visible = value; }
        bool getVisible() { return visible; }

};

#endif
#ifndef INCLUDED_NODE
#define INCLUDED_NODE

#include "raylib-cpp.hpp"
#include "vector"
#include "iostream"

const int MAX_CHILDREN = 100;

class Node {
    private:
        std::vector<Node*> children;
        
        Node* parent = NULL;
        Node* root = NULL;
        bool is_root = false;

        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};
    public:

        // - Core -
        void init(Node* root_node, Node* parent_node);
        virtual void process(float delta);

        // - Children-
        void addChild(Node* child);
        Node* getChild(int child_idx);
        std::vector<Node*> getChildren();
        int getChildCount();

        // - Hierarchy -
        bool isInsideTree();
        Node* getParent();
        int getIndex();
        int getId();
        Node* getRoot();
        void kill();

        // - Values -
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

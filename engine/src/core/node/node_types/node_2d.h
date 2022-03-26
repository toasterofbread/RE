#ifndef INCLUDED_NODE2D
#define INCLUDED_NODE2D

#include "engine/src/core/node/node.h"
#include "engine/src/core/signal.h"

#include "raylib-cpp.hpp"

class Node2D: public Node {

    public:

        Signal<void, Node2D*, int> SIGNAL_DRAW_LAYER_CHANGED;

        Node2D(): Node() { name = getTypeName(); }
        static string getTypeName() { return "Node2D"; }

        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_name);

        virtual void draw();

        void setModulate(Color value) { modulate = value; }
        Color getModulate() { return modulate; }

        void setDrawLayer(int value);
        int getDrawLayer(bool consider_parents = false);

        void setRelativeDrawLayer(bool value) { relative_draw_layer = value; }
        bool isRelativeDrawLayer() { return relative_draw_layer; }

        void setShowGizmos(bool value) { show_gizmos = value; }
        bool getShowGizmos() { return show_gizmos; }

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
        bool isVisible(bool consider_parents = false);


    private:
        Color modulate = RAYWHITE;

        int draw_layer = 0;
        bool relative_draw_layer = true;

        bool show_gizmos = false;
        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};
        float rotation = 0.0f;
        bool visible = true;

        void addedToNode(Node* parent_node);
        void removedFromNode(Node* former_parent_node);
        void onParentDrawLayerChanged(Node2D* _node, int old_draw_layer);
};

#endif
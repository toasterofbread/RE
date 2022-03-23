#ifndef INCLUDED_NODE2D
#define INCLUDED_NODE2D

#include "engine/src/core/node/node.h"

#include "raylib-cpp.hpp"

class Node2D: public Node {

    public:

        Node2D(Engine* engine_singleton): Node(engine_singleton) { name = getTypeName(); }
        virtual string getTypeName() {return "Node2D";}

        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_name, Engine* engine);

        void draw();

        void setModulate(Color value) { modulate = value; }
        Color getModulate() { return modulate; }

        void setDrawLayer(int value) { draw_layer = value; }
        int getDrawLayer() { return draw_layer; }

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
        bool getVisible() { return visible; }

        Color modulate = RAYWHITE;
        int draw_layer = 0;

        bool show_gizmos = false;
        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};
        float rotation = 0.0f;
        bool visible = true;
};

#endif
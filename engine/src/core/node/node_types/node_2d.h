#ifndef INCLUDED_NODE2D
#define INCLUDED_NODE2D

#include "engine/src/core/node/node.h"
#include "engine/src/core/signal.h"

#include "engine/src/raylib_include.h"

class Node2D: public Node {

    public:

        Signal<int, int> SIGNAL_DRAW_LAYER_CHANGED;

        Signal<Vector2> SIGNAL_GLOBAL_POSITION_CHANGED;
        Signal<float> SIGNAL_GLOBAL_ROTATION_CHANGED;
        Signal<Vector2> SIGNAL_GLOBAL_SCALE_CHANGED;

        REGISTER_NODE(Node2D);

        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_name);

        virtual void draw();

        void setModulate(Colour value) { modulate = value; }
        void setGlobalModulate(Colour value) {}
        Colour getModulate();
        Colour getGlobalModulate();

        void setUseRelativeModulate(bool value) { modulate_relative_to_parent = value; }
        bool getUseRelativeModulate() { return modulate_relative_to_parent; }

        void setDrawLayer(int value);
        void setGlobalDrawLayer(int value);
        int getDrawLayer();
        int getGlobalDrawLayer();

        void setRelativeDrawLayer(bool value) { relative_draw_layer = value; }
        bool isRelativeDrawLayer() { return relative_draw_layer; }

        void setShowGizmos(bool value) { show_gizmos = value; }
        bool getShowGizmos() { return show_gizmos; }

        virtual void setPosition(Vector2 value);
        Vector2 getPosition();
        void setGlobalPosition(Vector2 value);
        Vector2 getGlobalPosition();

        void setUseRelativePosition(bool value) { position_relative_to_parent = value; }
        bool getUseRelativePosition() { return position_relative_to_parent; }

        virtual void setScale(Vector2 value);
        Vector2 getScale();
        void setGlobalScale(Vector2 value);
        Vector2 getGlobalScale();

        void setUseRelativeScale(bool value) { 
            if (scale_relative_to_parent == value) {
                return;
            }
            
            Vector2 old_global_scale = getGlobalScale();
            scale_relative_to_parent = value; 
            SIGNAL_GLOBAL_SCALE_CHANGED.emit(old_global_scale);
        }
        bool getUseRelativeScale() { return scale_relative_to_parent; }

        virtual void setRotation(float value);
        float getRotation();
        void setGlobalRotation(float value);
        float getGlobalRotation();

        void setUseRelativeRotation(bool value) { rotation_relative_to_parent = value; }
        bool getUseRelativeRotation() { return rotation_relative_to_parent; }

        void setVisible(bool value);
        void setGlobalVisible(bool value);
        bool isVisible();
        bool isGlobalVisible();

        void setUseRelativeVisibility(bool value) { visible_relative_to_parent = value; }
        bool getUseRelativeVisibility() { return visible_relative_to_parent; }

    protected:
        void addedToNode(Node* parent_node);
        void removedFromNode(Node* former_parent_node);
        
        virtual void onParentGlobalPositionChanged(Vector2 old_global_position);
        virtual void onParentGlobalRotationChanged(float old_global_rotation);
        virtual void onParentGlobalScaleChanged(Vector2 old_global_scale);

    private:
        Colour modulate = WHITE;

        int draw_layer = 0;
        bool relative_draw_layer = true;

        bool show_gizmos = false;
        Vector2 position = Vector2{0, 0};
        Vector2 scale = Vector2{1, 1};
        float rotation = 0.0f; // Must be in radians
        bool visible = true;

        bool position_relative_to_parent = true;
        bool rotation_relative_to_parent = true;
        bool scale_relative_to_parent = true;
        bool modulate_relative_to_parent = true;
        bool visible_relative_to_parent = true;
        bool draw_layer_relative_to_parent = true;

        void onParentDrawLayerChanged(int old_draw_layer, int new_draw_layer);
};

#endif
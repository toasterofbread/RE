#ifndef INCLUDED_scene_tree
#define INCLUDED_scene_tree

#include <string>
#include <functional>
using namespace std;

#include "common/utils.h"

#include <vector>
using namespace std;

// Forward declaration
class Engine;
class Node;
class Node2D;
class Node3D;
class Timer;
class Camera2D;
class Camera3D;

class SceneTree {
    public:
        enum class STATE {
            PREINIT, IDLE, PROCESS, KILL_NODES, DRAW
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

        void setEnabledCamera2D(Camera2D* value);
        Camera2D* getEnabledCamera2D();

        void setEnabledCamera3D(Camera3D* value);
        Camera3D* getEnabledCamera3D();

        static const int MIN_DRAW_LAYER = -2048;
        static const int MAX_DRAW_LAYER = 2048;

        template<typename... Arguments>
        void addDrawFunction(int layer, bool one_shot, function<void(Arguments...)> draw_function, Arguments... arguments) {
            BaseDrawable* drawable = new Drawable<Arguments...>(draw_function, arguments...);
            drawable->one_shot = one_shot;
            drawable_nodes[layer - MIN_DRAW_LAYER].push_back(drawable);
        }

    private:
        Node* root_node;
        Camera2D* enabled_camera_2d = NULL;
        Camera3D* enabled_camera_3d = NULL;

        struct BaseDrawable {
            bool one_shot = false;
            virtual void draw() = 0;
            virtual bool compare(Node2D* node) = 0;
            virtual bool compare(Node3D* node) = 0;

            enum TYPE {
                NODE2D, NODE3D, FUNCTION
            };
            TYPE type;

            protected:
                void callNodeDraw();
                Node2D* node_2d = NULL;
                Node3D* node_3d = NULL;
        };

        template<typename... FuncArgs>
        struct Drawable: public BaseDrawable {
            Drawable(Node2D* node) {
                node_2d = node;
                type = NODE2D;
            }
            Drawable(Node3D* node) {
                node_3d = node;
                type = NODE3D;
            }
            Drawable(function<void(FuncArgs...)> func, FuncArgs... arguments): draw_function_args(forward<FuncArgs>(arguments)...) {
                draw_function = func;
                type = FUNCTION;
            }

            void draw() {
                switch (type) {
                    case NODE2D:
                    case NODE3D:
                        callNodeDraw(); break;
                    case FUNCTION:
                        callFunction(make_index_sequence<sizeof...(FuncArgs)>{}); break;
                }
            }

            bool compare(Node2D* node) {
                return node_2d == node;
            }

            bool compare(Node3D* node) {
                return node_3d == node;
            }

            bool compare(function<void(FuncArgs...)> func) {
                return draw_function == func;
            }

            private:
                function<void(FuncArgs...)> draw_function;
                tuple<FuncArgs...> draw_function_args;

                template<size_t... I>
                void callFunction(index_sequence<I...>) {
                    draw_function(get<I>(draw_function_args)...);
                }

        };
        list<BaseDrawable*> drawable_nodes[MAX_DRAW_LAYER - MIN_DRAW_LAYER];

        STATE current_state = STATE::PREINIT;
        vector<Node*> kill_queue;

        void onDrawableNodeLayerChanged(int old_draw_layer, int new_draw_layer, Node2D* node);
        void onDrawableNodeLayerChanged(int old_draw_layer, int new_draw_layer, Node3D* node);
};

#endif
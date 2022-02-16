#include <raylib-cpp.hpp>

#include "engine/src/core/node/scene_node.h"
#include "project/src/nodes/samus/states/state.h"

// Forward declarations
class AnimatedSprite;

class Samus: public SceneNode {
    private:
        string scene_path = "samus";
        AnimatedSprite* main_sprite;

        unordered_map<string, SamusState*> states;
        SamusState* current_state = NULL;
    public:
        Samus(NodeManager* node_manager, bool initialise = true);
        virtual string getTypeName() {return "Samus";}

        void ready();
        void process(float delta);
};
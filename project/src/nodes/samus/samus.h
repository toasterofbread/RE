#include <raylib-cpp.hpp>

#include "engine/src/core/node/scene_node.h"
#include "engine/src/input/input_event.h"
#include "engine/src/input/macro.h"

// Forward declarations
class AnimatedSprite;
class SamusState;

class Samus: public SceneNode {
    public:
        Samus(NodeManager* node_manager, bool initialise = true);
        virtual string getTypeName() {return "Samus";}

        void ready();
        void process(float delta);
        
    protected:
        AnimatedSprite* main_sprite;

        template<typename... Args>
        void changeToState(string state_key, Args... data);
        SamusState* getState(string state_key);

    private:
        unordered_map<string, SamusState*> states;
        SamusState* current_state = NULL;

        // Input events

};
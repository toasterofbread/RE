#include "samus.h"

#include <raylib-cpp.hpp>

#include "project/src/nodes/samus/states/state.h"
#include "engine/src/utils.h"
#include "engine/src/input/input.h"
#include "engine/src/core/node/animated_sprite.h"
#include "engine/src/input/input.h"

Samus::Samus(NodeManager* node_manager, bool initialise): SceneNode(node_manager) {
    name = getTypeName();
    if (initialise) {
        init_scenenode("project/src/nodes/samus/scene.ssg", "Samus");
    }

    // Create and register states
    vector<SamusState*> _states = {
        new SamusStateStand(this),
        new SamusStateRun(this)
    };
    for (auto i = _states.begin(); i != _states.end(); ++i) {
        states[(*i)->getName()] = *i;
    }

    current_state = states["standing"];
}

void test_emitted(void* pointer) {
    print("TEST EMITTED");
}

void Samus::ready() {
    SceneNode::ready();

    main_sprite = (AnimatedSprite*)getChild("MainSprite");
    main_sprite->play("run_aim_down_left");

}

void Samus::process(float delta) {
    SceneNode::process(delta);
    
    if (current_state != NULL) {
        current_state->process(delta);
    }

}

template<typename... Args>
void Samus::changeToState(string state_key, Args... data) {
    SamusState* target_state = getState(state_key);
    if (target_state == current_state) {
        warn("Samus's state is already '" + state_key + "'", true);
        return;
    }

    SamusState* previous_state = current_state;

    previous_state->stateDisabled(target_state, data...);
    current_state = target_state;
    target_state->stateEnabled(previous_state, data...);
    
}

SamusState* Samus::getState(string state_key) {
    return states[state_key];
}
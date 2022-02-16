#include "samus.h"

#include <raylib-cpp.hpp>

#include "engine/src/utils.h"
#include "engine/src/input/input.h"
#include "engine/src/core/node/animated_sprite.h"

Samus::Samus(NodeManager* node_manager, bool initialise): SceneNode(node_manager) {
    name = getTypeName();
    if (initialise) {
        init_scenenode("project/src/nodes/samus/scene.ssg", "Samus");
    }

    // Create and register states
    vector<SamusState*> _states = {
        new SamusStateStanding
    };
    for (auto i = _states.begin(); i != _states.end(); ++i) {
        states[(*i)->getName()] = *i;
    }

    current_state = states["standing"];
}

void Samus::ready() {
    SceneNode::ready();

    main_sprite = (AnimatedSprite*)getChild("MainSprite");
    main_sprite->play("run_aim_down_left");

    // Signal* SIGNAL_TEST = new Signal();
    print("WAITING FOR KILL");
    SIGNAL_KILLED->await();
    print("KILL EMITTED");
}

float wait = 0;
void Samus::process(float delta) {
    SceneNode::process(delta);
    
    if (current_state != NULL) {
        current_state->process(delta);
    }

    // setPosition(aV(getPosition(), getPadVector(false, delta * 600.0f)));
}
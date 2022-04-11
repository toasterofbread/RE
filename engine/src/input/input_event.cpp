#include "input_event.h"

#include "engine/src/engine.h"
#include "common/utils.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/scene_tree.h"

void InputEvent::init() {
    Engine::getSingleton()->inputEventCreated(this);
}

bool InputEvent::isTriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isPressed()) {
            return true;
        }
    }
    return false;
}

bool InputEvent::isJustTriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isJustPressed()) {
            return true;
        }
    }
    return false;
}

bool InputEvent::isJustUntriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isJustReleased()) {
            return true;
        }
    }
    return false;
}

void InputEvent::process(float delta) {
    if (SIGNAL_TRIGGERED.getConnectionCount() > 0 && isJustTriggered()) {
        SIGNAL_TRIGGERED.emit();
    }
    else if (SIGNAL_UNTRIGGERED.getConnectionCount() > 0 && isJustUntriggered()) {
        SIGNAL_UNTRIGGERED.emit();
    }
}

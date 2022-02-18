#include "input_event.h"

#include "engine/src/utils.h"
#include "engine/src/engine.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node_manager.h"

void InputEvent::init() {
    engine->inputEventCreated(this);
    SIGNAL_TRIGGERED = new Signal<void>();
    SIGNAL_UNTRIGGERED = new Signal<void>();
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
    if (SIGNAL_TRIGGERED->connected() && isJustTriggered()) {
        SIGNAL_TRIGGERED->emit();
    }
    else if (SIGNAL_UNTRIGGERED->connected() && isJustUntriggered()) {
        SIGNAL_UNTRIGGERED->emit();
    }
}

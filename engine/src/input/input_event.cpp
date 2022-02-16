#include "input_event.h"

#include "engine/src/input/macro.h"

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
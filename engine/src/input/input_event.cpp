#include "input_event.h"

#include "engine/src/engine.h"
#include "common/utils.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/node/scene_tree.h"

bool InputEvent::isTriggered() {
    switch (hold_type) {
        case HOLD_TYPE::DELAY: return hold_status > 0;
        case HOLD_TYPE::REPEAT: return false;
        default: return IisTriggered();
    }
}

bool InputEvent::isJustTriggered() {
    switch (hold_type) {
        case HOLD_TYPE::DELAY: return hold_status == 1;
        case HOLD_TYPE::REPEAT: return hold_status == 1;
        default: return IisJustTriggered();
    }
}

bool InputEvent::isJustUntriggered() {
    switch (hold_type) {
        case HOLD_TYPE::DELAY: return hold_status == -1;
        case HOLD_TYPE::REPEAT: return false;
        default: return IisJustTriggered();
    }
}

bool InputEvent::IisTriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isPressed()) {
            return true;
        }
    }
    return false;
}

bool InputEvent::IisJustTriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isJustPressed()) {
            return true;
        }
    }
    return false;
}

bool InputEvent::IisJustUntriggered() {
    for (auto i = macros.begin(); i != macros.end(); ++i) {
        if ((*i)->isJustReleased()) {
            return true;
        }
    }
    return false;
}

void InputEvent::process(float delta) {

    if (hold_type == HOLD_TYPE::NONE) {
        if (SIGNAL_TRIGGERED.getConnectionCount() > 0 && isJustTriggered()) {
            SIGNAL_TRIGGERED.emit();
        }
        else if (SIGNAL_UNTRIGGERED.getConnectionCount() > 0 && isJustUntriggered()) {
            SIGNAL_UNTRIGGERED.emit();
        }
    }
    else if (!IisTriggered()) {
        if (hold_status != 0) {
            if (hold_status == -1) {
                hold_status = 0;
            }
            else {
                SIGNAL_UNTRIGGERED.emit();
                hold_status = -1;
                hold_time = 0.0f;
            }
        }
    }
    else if (hold_type == HOLD_TYPE::REPEAT && hold_status == 0 || hold_status == -1) {
        hold_status = 1;
        SIGNAL_TRIGGERED.emit();
    }
    else {
        hold_time += delta;
        if (hold_status != 2 || hold_type == HOLD_TYPE::REPEAT) {
            if (hold_status == 1) {
                hold_status = 2;
            }
            else if (hold_time >= hold_interval) {
                SIGNAL_TRIGGERED.emit();
                if (hold_type == HOLD_TYPE::DELAY) {
                    hold_status = 1;
                }
                else {
                    hold_status = 1;
                    hold_time = 0.0f;
                }
            }
        }
    }
}

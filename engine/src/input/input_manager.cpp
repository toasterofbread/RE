#include "input_manager.h"

#include <vector>
using namespace std;

#include "common/utils.h"
#include "engine/src/engine.h"
#include "engine/src/input/input_event.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/node/scene_tree.h"
#include "common/input.h"

InputManager* InputManager::singleton = NULL;

InputManager::InputManager() {
    ASSERT(singleton == NULL);
    singleton = this;

    Engine* engine = Engine::getSingleton();

    INPUTEVENT_PAD_UP = new InputEvent({Macro::create_kb({Input::KEY_ARROW_UP}), Macro::create_kb({Input::KEY_W}), Macro::create_pad({Input::PAD_UP})});
    INPUTEVENT_PAD_DOWN = new InputEvent({Macro::create_kb({Input::KEY_ARROW_DOWN}), Macro::create_kb({Input::KEY_S}), Macro::create_pad({Input::PAD_DOWN})});
    INPUTEVENT_PAD_LEFT = new InputEvent({Macro::create_kb({Input::KEY_ARROW_LEFT}), Macro::create_kb({Input::KEY_A}), Macro::create_pad({Input::PAD_LEFT})});
    INPUTEVENT_PAD_RIGHT = new InputEvent({Macro::create_kb({Input::KEY_ARROW_RIGHT}), Macro::create_kb({Input::KEY_D}), Macro::create_pad({Input::PAD_RIGHT})});

    INPUTEVENT_REBUILD_AND_RUN = new InputEvent({Macro::create_kb({Input::KEY_F5})});
    INPUTEVENT_REBUILD_AND_RUN->SIGNAL_TRIGGERED.connect(&Engine::rebuildAndRun, Engine::getSingleton());
}

InputManager* InputManager::getSingleton() {
    ASSERT(singleton != NULL);
    return singleton;
}

void InputManager::printPressedKey() {
    int key = GetKeyPressed();
    if (key != 0) {
        OS::print("Key pressed: " + (string)to_string(key));
    }
}

Vector2 InputManager::getPadVector(bool just_pressed) {
    Vector2 ret = Vector2(0, 0);

    if (just_pressed) {
        ret.x -= INPUTEVENT_PAD_LEFT->isJustTriggered();
        ret.x += INPUTEVENT_PAD_RIGHT->isJustTriggered();
        ret.y -= INPUTEVENT_PAD_UP->isJustTriggered();
        ret.y += INPUTEVENT_PAD_DOWN->isJustTriggered();
    }
    else {
        ret.x -= INPUTEVENT_PAD_LEFT->isTriggered();
        ret.x += INPUTEVENT_PAD_RIGHT->isTriggered();
        ret.y -= INPUTEVENT_PAD_UP->isTriggered();
        ret.y += INPUTEVENT_PAD_DOWN->isTriggered();
    }

    return ret;
}

Vector2 InputManager::getAnalogPadVector(SIDE side, float deadzone) {
    Vector2 ret;
    if (side == LEFT) {
        ret = Vector2(Input::getAxis(Input::LX), Input::getAxis(Input::LY));
    }
    else {
        ret = Vector2(Input::getAxis(Input::RX), Input::getAxis(Input::RY));
    }

    ret.applyDeadzone(deadzone);
    return ret;
}

// void InputManager::addMacro(function<void(void)> func, vector<InputManager::GAMEPAD_BUTTON> gamepad_button_combination, string label, bool display_label) {
//     gamepad_macros.push_back(make_tuple(func, gamepad_button_combination, label, display_label));
// }
// void InputManager::addMacro(function<void(void)> func, vector<InputManager::KEYBOARD_BUTTON> keyboard_button_combination, string label, bool display_label) {
//     keyboard_macros.push_back(make_tuple(func, keyboard_button_combination, label, display_label));
// }

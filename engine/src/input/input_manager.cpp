#include "input_manager.h"

#include <raylib-cpp.hpp>
#include <vector>
using namespace std;

#include "engine/src/utils.h"
#include "engine/src/engine.h"
#include "engine/src/input/input_event.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node_manager.h"

InputManager::InputManager(Engine* engine_singleton) {
    engine = engine_singleton;
}

void InputManager::init() {
    InputEvent* INPUTEVENT_PAD_UP = new InputEvent(engine, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_UP})});
    InputEvent* INPUTEVENT_PAD_DOWN = new InputEvent(engine, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_DOWN})});
    InputEvent* INPUTEVENT_PAD_LEFT = new InputEvent(engine, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_LEFT})});
    InputEvent* INPUTEVENT_PAD_RIGHT = new InputEvent(engine, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_RIGHT})});
    InputEvent* INPUTEVENT_REBUILD_AND_RUN = new InputEvent(engine, {new Macro(vector<KEYBOARD_BUTTON>{F5})});
    INPUTEVENT_REBUILD_AND_RUN->SIGNAL_TRIGGERED->connect<Engine>(&Engine::rebuildAndRun, engine);
}

void InputManager::process(float delta) {
}

// void InputManager::inputProcess(float delta) {

//     auto printMacroInfo = [](string macro_name, bool gamepad) {
//         if (macro_name.empty()) return;
//         string msg = (gamepad ? "Gamepad" : "Keyboard") + (string)" macro triggered: " + macro_name;
//         print(encaseStringInBox(msg));
//     };

//     // !todo
// }

void InputManager::printPressedKey() {
    int key = GetKeyPressed();
    if (key != 0) {
        print("Key pressed: " + (string)int2char(key));
    }
}

bool InputManager::isButtonPressed(InputManager::GAMEPAD_BUTTON button) {
    return IsGamepadButtonDown(0, button);
}
bool InputManager::isButtonJustPressed(InputManager::GAMEPAD_BUTTON button){
    return IsGamepadButtonPressed(0, button);
}
bool InputManager::isButtonJustReleased(InputManager::GAMEPAD_BUTTON button){
    return IsGamepadButtonReleased(0, button);
}

bool InputManager::isButtonPressed(InputManager::KEYBOARD_BUTTON key) {
    return IsKeyDown(key);
}
bool InputManager::isButtonJustPressed(InputManager::KEYBOARD_BUTTON key) {
    return IsKeyPressed(key);
}
bool InputManager::isButtonJustReleased(InputManager::KEYBOARD_BUTTON key) {
    return IsKeyReleased(key);
}

bool InputManager::isKeyModifier(InputManager::KEYBOARD_BUTTON key) {
    vector<KEYBOARD_BUTTON> modifier_keys = {LCTRL, LALT, LSHIFT};
    return vectorContainsValue(&modifier_keys, key);
}
bool InputManager::isKeyModifier(InputManager::GAMEPAD_BUTTON key) {
    return false;
}

Vector2 InputManager::getPadVector(bool just_pressed, float delta) {
    Vector2 ret = Vector2{0, 0};

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

    ret = sign(ret);

    return mV(ret, delta);
}

// void InputManager::addMacro(function<void(void)> func, vector<InputManager::GAMEPAD_BUTTON> gamepad_button_combination, string label, bool display_label) {
//     gamepad_macros.push_back(make_tuple(func, gamepad_button_combination, label, display_label));
// }
// void InputManager::addMacro(function<void(void)> func, vector<InputManager::KEYBOARD_BUTTON> keyboard_button_combination, string label, bool display_label) {
//     keyboard_macros.push_back(make_tuple(func, keyboard_button_combination, label, display_label));
// }

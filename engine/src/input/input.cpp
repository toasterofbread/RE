#include "input.h"

#include <raylib-cpp.hpp>
#include <vector>
using namespace std;

#include "engine/src/utils.h"
#include "engine/src/input/input_event.h"
#include "engine/src/input/macro.h"
#include "engine/src/core/signal.h"
#include "engine/src/core/node/node_manager.h"

Input::Input(NodeManager* node_manager) {

    manager = node_manager;

    InputEvent* INPUTEVENT_PAD_UP = new InputEvent(manager, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_UP})});
    InputEvent* INPUTEVENT_PAD_DOWN = new InputEvent(manager, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_DOWN})});
    InputEvent* INPUTEVENT_PAD_LEFT = new InputEvent(manager, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_LEFT})});
    InputEvent* INPUTEVENT_PAD_RIGHT = new InputEvent(manager, {new Macro(vector<KEYBOARD_BUTTON>{ARROW_RIGHT})});

    InputEvent* INPUTEVENT_REBUILD_AND_RUN = new InputEvent(manager, {new Macro(vector<KEYBOARD_BUTTON>{F5})});
    INPUTEVENT_REBUILD_AND_RUN->SIGNAL_TRIGGERED->connect<NodeManager>(&NodeManager::rebuildAndRun, manager);
}

void Input::process(float delta) {
}

// void Input::inputProcess(float delta) {

//     auto printMacroInfo = [](string macro_name, bool gamepad) {
//         if (macro_name.empty()) return;
//         string msg = (gamepad ? "Gamepad" : "Keyboard") + (string)" macro triggered: " + macro_name;
//         print(encaseStringInBox(msg));
//     };

//     // !todo
// }

void Input::printPressedKey() {
    int key = GetKeyPressed();
    if (key != 0) {
        print("Key pressed: " + (string)int2char(key));
    }
}

bool Input::isButtonPressed(Input::GAMEPAD_BUTTON button) {
    return IsGamepadButtonDown(0, button);
}
bool Input::isButtonJustPressed(Input::GAMEPAD_BUTTON button){
    return IsGamepadButtonPressed(0, button);
}
bool Input::isButtonJustReleased(Input::GAMEPAD_BUTTON button){
    return IsGamepadButtonReleased(0, button);
}

bool Input::isButtonPressed(Input::KEYBOARD_BUTTON key) {
    return IsKeyDown(key);
}
bool Input::isButtonJustPressed(Input::KEYBOARD_BUTTON key) {
    return IsKeyPressed(key);
}
bool Input::isButtonJustReleased(Input::KEYBOARD_BUTTON key) {
    return IsKeyReleased(key);
}

bool Input::isKeyModifier(Input::KEYBOARD_BUTTON key) {
    vector<KEYBOARD_BUTTON> modifier_keys = {LCTRL, LALT, LSHIFT};
    return vectorContainsValue(&modifier_keys, key);
}
bool Input::isKeyModifier(Input::GAMEPAD_BUTTON key) {
    return false;
}

Vector2 Input::getPadVector(bool just_pressed, float delta) {
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

// void Input::addMacro(function<void(void)> func, vector<Input::GAMEPAD_BUTTON> gamepad_button_combination, string label, bool display_label) {
//     gamepad_macros.push_back(make_tuple(func, gamepad_button_combination, label, display_label));
// }
// void Input::addMacro(function<void(void)> func, vector<Input::KEYBOARD_BUTTON> keyboard_button_combination, string label, bool display_label) {
//     keyboard_macros.push_back(make_tuple(func, keyboard_button_combination, label, display_label));
// }

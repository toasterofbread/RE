#include "input.h"
#include <raylib-cpp.hpp>

#include "engine/src/utils.h"

vector<std::tuple<function<void(void)>, vector<GAMEPAD_BUTTON>, string, bool>> gamepad_macros;
vector<std::tuple<function<void(void)>, vector<KEYBOARD_BUTTON>, string, bool>> keyboard_macros;
vector<KEYBOARD_BUTTON> modifier_keys = {LCTRL, LALT, LSHIFT};

void inputProcess(float delta) {

    auto printMacroInfo = [](string macro_name, bool gamepad) {
        if (macro_name.empty()) return;
        string msg = (gamepad ? "Gamepad" : "Keyboard") + (string)" macro triggered: " + macro_name;
        print(encaseStringInBox(msg));
    };

    // !todo
}

void printPressedKey() {
    int key = GetKeyPressed();
    if (key != 0) {
        print("Key pressed: " + (string)int2char(key));
    }
}

bool isButtonPressed(GAMEPAD_BUTTON button) {
    return IsGamepadButtonDown(gamepad_id, button);
}
bool isButtonJustPressed(GAMEPAD_BUTTON button){
    return IsGamepadButtonPressed(gamepad_id, button);
}
bool isButtonJustReleased(GAMEPAD_BUTTON button){
    return IsGamepadButtonReleased(gamepad_id, button);
}

bool isButtonPressed(KEYBOARD_BUTTON key) {
    return IsKeyDown(key);
}
bool isButtonJustPressed(KEYBOARD_BUTTON key) {
    return IsKeyPressed(key);
}
bool isButtonJustReleased(KEYBOARD_BUTTON key) {
    return IsKeyReleased(key);
}

bool isKeyModifier(KEYBOARD_BUTTON key) {
    return vectorContainsValue(&modifier_keys, key);
}
bool isKeyModifier(GAMEPAD_BUTTON key) {
    return false;
}

Vector2 getPadVector(bool just_pressed, float delta) {
    Vector2 ret = Vector2{0, 0};

    if (just_pressed) {
        ret.x -= isButtonJustPressed(PAD_LEFT);
        ret.x += isButtonJustPressed(PAD_RIGHT);
        ret.y -= isButtonJustPressed(PAD_UP);
        ret.y += isButtonJustPressed(PAD_DOWN);

        ret.x -= isButtonJustPressed(ARROW_LEFT);
        ret.x += isButtonJustPressed(ARROW_RIGHT);
        ret.y -= isButtonJustPressed(ARROW_UP);
        ret.y += isButtonJustPressed(ARROW_DOWN);
    }
    else {
        ret.x -= isButtonPressed(PAD_LEFT);
        ret.x += isButtonPressed(PAD_RIGHT);
        ret.y -= isButtonPressed(PAD_UP);
        ret.y += isButtonPressed(PAD_DOWN);

        ret.x -= isButtonPressed(ARROW_LEFT);
        ret.x += isButtonPressed(ARROW_RIGHT);
        ret.y -= isButtonPressed(ARROW_UP);
        ret.y += isButtonPressed(ARROW_DOWN);
    }

    ret = sign(ret);

    return mV(ret, delta);
}


void addMacro(function<void(void)> func, vector<GAMEPAD_BUTTON> button_combination, string label, bool display_label) {
    gamepad_macros.push_back(make_tuple(func, button_combination, label, display_label));
}
void addMacro(function<void(void)> func, vector<KEYBOARD_BUTTON> key_combination, string label, bool display_label) {
    keyboard_macros.push_back(make_tuple(func, key_combination, label, display_label));
}

#include "macro.h"

#include "engine/src/input/input.h"
#include "engine/src/utils.h"

Macro::Macro(vector<KEYBOARD_BUTTON> buttons) {
    type = MACRO_TYPE_KEYBOARD;
    keyboard_buttons = buttons;

    all_modifiers = true;
    for (auto i = keyboard_buttons.begin(); i != keyboard_buttons.end(); ++i) {
        if (!isKeyModifier(*i)) {
            all_modifiers = false;
            break;
        }
    }
}

Macro::Macro(vector<GAMEPAD_BUTTON> buttons) {
    type = MACRO_TYPE_GAMEPAD;
    gamepad_buttons = buttons;
}

bool Macro::isPressed() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetPressed(keyboard_buttons);
        case MACRO_TYPE_GAMEPAD: return isButtonSetPressed(gamepad_buttons);
    }
    return false;
}
bool Macro::isJustPressed() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetJustPressed(keyboard_buttons, all_modifiers);
        case MACRO_TYPE_GAMEPAD: return isButtonSetJustPressed(gamepad_buttons, all_modifiers);
    }
    return false;
}
bool Macro::isJustReleased() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetJustReleased(keyboard_buttons);
        case MACRO_TYPE_GAMEPAD: return isButtonSetJustReleased(gamepad_buttons);
    }
    return false;
}

template<typename T>
bool Macro::isButtonSetPressed(vector<T> buttons) {
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!isButtonPressed(*i)) {
            return false;
        }
    }
    return true;
}
template<typename T>
bool Macro::isButtonSetJustPressed(vector<T> buttons, bool all_are_modifiers) {
    bool just_pressed = false;
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!isButtonPressed(*i)) {
            return false;
        }
        if (isButtonJustPressed(*i) && (all_are_modifiers || !isKeyModifier(*i))) {
            just_pressed = true;
        }
    }
    return just_pressed;
}
template<typename T>
bool Macro::isButtonSetJustReleased(vector<T> buttons) {
    bool released = false;
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!isButtonPressed(*i) && !isButtonJustReleased(*i)) {
            return false;
        }
        if (isButtonJustReleased(*i)) {
            released = true;
        }
    }
    return released;
}
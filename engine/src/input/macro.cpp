#include "macro.h"

#include "common/utils.h"
#include "common/input.h"

Macro::Macro(vector<Input::KeyboardButton> _keyboard_buttons) {
    type = MACRO_TYPE_KEYBOARD;
    keyboard_buttons = _keyboard_buttons;

    all_modifiers = true;
    for (auto i = keyboard_buttons.begin(); i != keyboard_buttons.end(); ++i) {
        if (!Input::isKeyModifier(*i)) {
            all_modifiers = false;
            break;
        }
    }
}

Macro::Macro(vector<Input::GamepadButton> _gamepad_buttons) {
    type = MACRO_TYPE_GAMEPAD;
    gamepad_buttons = _gamepad_buttons;
}

Macro::Macro(vector<Input::KeyboardButton> _keyboard_buttons, vector<Input::GamepadButton> _gamepad_buttons) {
    type = MACRO_TYPE_BOTH;
    keyboard_buttons = _keyboard_buttons;
    gamepad_buttons = _gamepad_buttons;

    all_modifiers = true;
    for (auto i = keyboard_buttons.begin(); i != keyboard_buttons.end(); ++i) {
        if (!Input::isKeyModifier(*i)) {
            all_modifiers = false;
            break;
        }
    }
}

bool Macro::isPressed() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetPressed(keyboard_buttons);
        case MACRO_TYPE_GAMEPAD: return isButtonSetPressed(gamepad_buttons);
        case MACRO_TYPE_BOTH: return isButtonSetPressed(keyboard_buttons) || isButtonSetPressed(gamepad_buttons);
    }
    return false;
}
bool Macro::isJustPressed() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetJustPressed(keyboard_buttons, all_modifiers);
        case MACRO_TYPE_GAMEPAD: return isButtonSetJustPressed(gamepad_buttons, all_modifiers);
        case MACRO_TYPE_BOTH: return isButtonSetJustPressed(keyboard_buttons, all_modifiers) || isButtonSetJustPressed(gamepad_buttons, all_modifiers);
    }
    return false;
}
bool Macro::isJustReleased() {
    switch (type) {
        case MACRO_TYPE_KEYBOARD: return isButtonSetJustReleased(keyboard_buttons);
        case MACRO_TYPE_GAMEPAD: return isButtonSetJustReleased(gamepad_buttons);
        case MACRO_TYPE_BOTH: return isButtonSetJustReleased(keyboard_buttons) || isButtonSetJustReleased(gamepad_buttons);
    }
    return false;
}

template<typename T>
bool Macro::isButtonSetPressed(vector<T> buttons) {
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!Input::isButtonPressed(*i)) {
            return false;
        }
    }
    return true;
}
template<typename T>
bool Macro::isButtonSetJustPressed(vector<T> buttons, bool all_are_modifiers) {
    bool just_pressed = false;
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!Input::isButtonPressed(*i)) {
            return false;
        }
        if (Input::isButtonJustPressed(*i) && (all_are_modifiers || !Input::isKeyModifier(*i))) {
            just_pressed = true;
        }
    }
    return just_pressed;
}
template<typename T>
bool Macro::isButtonSetJustReleased(vector<T> buttons) {
    bool released = false;
    for (auto i = buttons.begin(); i != buttons.end(); ++i) {
        if (!Input::isButtonPressed(*i) && !Input::isButtonJustReleased(*i)) {
            return false;
        }
        if (Input::isButtonJustReleased(*i)) {
            released = true;
        }
    }
    return released;
}

Macro* Macro::create_kb(vector<Input::KeyboardButton> keyboard_buttons) {
    return new Macro(keyboard_buttons);
}
Macro* Macro::create_pad(vector<Input::GamepadButton> gamepad_buttons) {
    return new Macro(gamepad_buttons);
}
Macro* Macro::create_dual(vector<Input::KeyboardButton> keyboard_buttons, vector<Input::GamepadButton> gamepad_buttons) {
    return new Macro(keyboard_buttons, gamepad_buttons);
}
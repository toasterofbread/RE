#include "macro.h"

#include "common/utils.h"
#include "common/input.h"

Macro* Macro::create() {
    return new Macro;
}

Macro* Macro::setKb(vector<Input::KeyboardButton> buttons) {
    keyboard_buttons = buttons;
    has_kb = true;

    all_modifiers = true;
    for (Input::KeyboardButton button : keyboard_buttons) {
        if (!Input::isKeyModifier(button)) {
            all_modifiers = false;
            break;
        }
    }

    return this;
}
Macro* Macro::setPad(vector<Input::GamepadButton> buttons) {
    gamepad_buttons = buttons;
    has_pad = true;
    return this;
}
Macro* Macro::setMouse(vector<Input::MouseButton> buttons) {
    mouse_buttons = buttons;
    has_mouse = true;
    return this;
}

bool Macro::isPressed() {
    if (has_kb && isButtonSetPressed(keyboard_buttons))
        return true;
    if (has_pad && isButtonSetPressed(gamepad_buttons))
        return true;
    if (has_mouse && isButtonSetPressed(mouse_buttons))
        return true;
    return false;
}

bool Macro::isJustPressed() {
    if (has_kb && isButtonSetJustPressed(keyboard_buttons, all_modifiers))
        return true;
    if (has_pad && isButtonSetJustPressed(gamepad_buttons, all_modifiers))
        return true;
    if (has_mouse && isButtonSetJustPressed(mouse_buttons, all_modifiers))
        return true;
    return false;
}

bool Macro::isJustReleased() {
    if (has_kb && isButtonSetJustReleased(keyboard_buttons))
        return true;
    if (has_pad && isButtonSetJustReleased(gamepad_buttons))
        return true;
    if (has_mouse && isButtonSetJustReleased(mouse_buttons))
        return true;
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

#include "common/input.h"

#include "common/raylib.h"

bool Input::isButtonPressed(GamepadButton button) {
    return IsGamepadButtonDown(0, button);
}
bool Input::isButtonPressed(KeyboardButton button) {
    return IsKeyDown(button);
}

bool Input::isButtonJustPressed(GamepadButton button) {
    return IsGamepadButtonPressed(0, button);
}
bool Input::isButtonJustPressed(KeyboardButton button) {
    return IsKeyPressed(button);
}

bool Input::isButtonJustReleased(GamepadButton button) {
    return IsGamepadButtonReleased(0, button);
}
bool Input::isButtonJustReleased(KeyboardButton button) {
    return IsKeyReleased(button);
}

float Input::getAxis(Axis axis) {
    switch (axis) {
        case Axis::LX:
            return GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        case Axis::LY:
            return GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        case Axis::RX:
            return GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
        case Axis::RY:
            return GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
        default:
            return 0.0f;
    }
}

bool Input::isKeyModifier(KeyboardButton key) {
    switch (key) {
        case KEY_LCTRL:
        case KEY_LALT:
        case KEY_LSHIFT:
            return true;
    }
    return false;
}
bool Input::isKeyModifier(GamepadButton key) {
    return false;
}
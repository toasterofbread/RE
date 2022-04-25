#include "common/input.h"

#include "common/raylib.h"
#include "common/utils.h"
#include "input/input_event.h"
#include "input/macro.h"

InputEvent INPUTEVENT_PAD_UP = InputEvent({Macro::create_kb({Input::KEY_ARROW_UP}), Macro::create_kb({Input::KEY_W}), Macro::create_pad({Input::PAD_UP})});
InputEvent INPUTEVENT_PAD_DOWN = InputEvent({Macro::create_kb({Input::KEY_ARROW_DOWN}), Macro::create_kb({Input::KEY_S}), Macro::create_pad({Input::PAD_DOWN})});
InputEvent INPUTEVENT_PAD_LEFT = InputEvent({Macro::create_kb({Input::KEY_ARROW_LEFT}), Macro::create_kb({Input::KEY_A}), Macro::create_pad({Input::PAD_LEFT})});
InputEvent INPUTEVENT_PAD_RIGHT = InputEvent({Macro::create_kb({Input::KEY_ARROW_RIGHT}), Macro::create_kb({Input::KEY_D}), Macro::create_pad({Input::PAD_RIGHT})});

bool Input::isButtonPressed(GamepadButton button) {
    return IsGamepadButtonDown(0, button);
}
bool Input::isButtonPressed(KeyboardButton button) {
    return IsKeyDown(button);
}
bool Input::isButtonPressed(MouseButton button) {
    switch (button) {
        case MouseButton::MOUSE_WHEEL_UP:
            return GetMouseWheelMove() == 1.0f;
        case MouseButton::MOUSE_WHEEL_DOWN:
            return GetMouseWheelMove() == -1.0f;
        default:
            return IsMouseButtonDown(button);
    }
}

bool Input::isButtonJustPressed(GamepadButton button) {
    return IsGamepadButtonPressed(0, button);
}
bool Input::isButtonJustPressed(KeyboardButton button) {
    return IsKeyPressed(button);
}
bool Input::isButtonJustPressed(MouseButton button) {
    switch (button) {
        case MouseButton::MOUSE_WHEEL_UP:
            return GetMouseWheelMove() == 1.0f;
        case MouseButton::MOUSE_WHEEL_DOWN:
            return GetMouseWheelMove() == -1.0f;
        default:
            return IsMouseButtonPressed(button);
    }
}

bool Input::isButtonJustReleased(GamepadButton button) {
    return IsGamepadButtonReleased(0, button);
}
bool Input::isButtonJustReleased(KeyboardButton button) {
    return IsKeyReleased(button);
}
bool Input::isButtonJustReleased(MouseButton button) {
    ASSERT(button != MouseButton::MOUSE_WHEEL_UP && button != MouseButton::MOUSE_WHEEL_DOWN);
    return IsMouseButtonReleased(button);
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

void Input::printPressedKey() {
    int key = GetKeyPressed();
    if (key != 0) {
        OS::print("Key pressed: " + (string)to_string(key));
    }
}

Vector2 Input::getPadVector(bool just_pressed) {
    Vector2 ret = Vector2(0, 0);

    if (just_pressed) {
        ret.x -= INPUTEVENT_PAD_LEFT.isJustTriggered();
        ret.x += INPUTEVENT_PAD_RIGHT.isJustTriggered();
        ret.y -= INPUTEVENT_PAD_UP.isJustTriggered();
        ret.y += INPUTEVENT_PAD_DOWN.isJustTriggered();
    }
    else {
        ret.x -= INPUTEVENT_PAD_LEFT.isTriggered();
        ret.x += INPUTEVENT_PAD_RIGHT.isTriggered();
        ret.y -= INPUTEVENT_PAD_UP.isTriggered();
        ret.y += INPUTEVENT_PAD_DOWN.isTriggered();
    }

    return ret;
}

Vector2 Input::getAnalogPadVector(SIDE side, float deadzone) {
    Vector2 ret;
    if (side == SIDE::LEFT) {
        ret = Vector2(Input::getAxis(Input::LX), Input::getAxis(Input::LY));
    }
    else {
        ret = Vector2(Input::getAxis(Input::RX), Input::getAxis(Input::RY));
    }

    ret.applyDeadzone(deadzone);
    return ret;
}

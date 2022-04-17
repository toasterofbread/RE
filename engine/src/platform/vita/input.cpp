#include "common/input.h"

#include <psp2/ctrl.h>
using namespace std;

unsigned int prev_buttons;
SceCtrlData ctrl;

#define hasBtn(btns, button) (btns & button) == button
#define nhasBtn(btns, button) (btns & button) != button

void Input::process() {
    prev_buttons = ctrl.buttons;
    sceCtrlPeekBufferPositive(0, &ctrl, 1);
}

float Input::getAxis(Axis axis) {
    switch (axis) {
        case Axis::LX:
            return ((float)ctrl.lx - 127.5f) / 127.5f;
        case Axis::LY:
            return ((float)ctrl.ly - 127.5f) / 127.5f;
        case Axis::RX:
            return ((float)ctrl.rx - 127.5f) / 127.5f;
        case Axis::RY:
            return ((float)ctrl.ry - 127.5f) / 127.5f;
        default:
            return 0.0f;
    }
}

bool Input::isButtonPressed(GamepadButton button) {
    return hasBtn(ctrl.buttons, button);
}
bool Input::isButtonPressed(KeyboardButton button) {
    return false;
}

bool Input::isButtonJustPressed(GamepadButton button) {
    return nhasBtn(prev_buttons, button) && hasBtn(ctrl.buttons, button);
}
bool Input::isButtonJustPressed(KeyboardButton button) {
    return false;
}

bool Input::isButtonJustReleased(GamepadButton button) {
    return hasBtn(prev_buttons, button) && nhasBtn(ctrl.buttons, button);
}
bool Input::isButtonJustReleased(KeyboardButton button) {
    return false;
}

bool Input::isKeyModifier(KeyboardButton key) {
    return false;
}
bool Input::isKeyModifier(GamepadButton key) {
    return false;
}
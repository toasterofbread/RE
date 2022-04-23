#ifndef INCLUDED_INPUT
#define INCLUDED_INPUT

#include "defs.h"

#ifdef GamepadButton
#undef GamepadButton
#endif

namespace Input {

    static void* data;

    INPUT_DEFS

    float getAxis(Axis axis);

    bool isButtonPressed(GamepadButton button);
    bool isButtonPressed(KeyboardButton button);
    bool isButtonPressed(MouseButton button);

    bool isButtonJustPressed(GamepadButton button);
    bool isButtonJustPressed(KeyboardButton button);
    bool isButtonJustPressed(MouseButton button);

    bool isButtonJustReleased(GamepadButton button);
    bool isButtonJustReleased(KeyboardButton button);
    bool isButtonJustReleased(MouseButton button);

    bool isKeyModifier(KeyboardButton key);
    bool isKeyModifier(GamepadButton key);
    bool isKeyModifier(MouseButton key);

};

#endif
#ifndef INCLUDED_INPUT
#define INCLUDED_INPUT

#include "defs.h"

#ifdef GamepadButton
#undef GamepadButton
#endif

namespace Input {

    static void* data;

    INPUT_DEFS

    #if INPUT_HAS_PROCESS
    void process();
    #endif

    float getAxis(Axis axis);

    bool isButtonPressed(GamepadButton button);
    bool isButtonPressed(KeyboardButton button);

    bool isButtonJustPressed(GamepadButton button);
    bool isButtonJustPressed(KeyboardButton button);

    bool isButtonJustReleased(GamepadButton button);
    bool isButtonJustReleased(KeyboardButton button);

    bool isKeyModifier(KeyboardButton key);
    bool isKeyModifier(GamepadButton key);

};

#endif
#ifndef INCLUDED_MACRO
#define INCLUDED_MACRO

#include "common/input.h"

#include <vector>
using namespace std;

class Macro {
    public:
        bool isPressed();
        bool isJustPressed();
        bool isJustReleased();

        template<typename T>
        bool isButtonSetPressed(vector<T> buttons);
        template<typename T>
        bool isButtonSetJustPressed(vector<T> buttons, bool all_are_modifiers);
        template<typename T>
        bool isButtonSetJustReleased(vector<T> buttons);
    
        static Macro* create_kb(vector<Input::KeyboardButton> keyboard_buttons);
        static Macro* create_pad(vector<Input::GamepadButton> gamepad_buttons);
        static Macro* create_dual(vector<Input::KeyboardButton> keyboard_buttons, vector<Input::GamepadButton> gamepad_buttons);

    private:

        enum MACRO_TYPE {MACRO_TYPE_KEYBOARD, MACRO_TYPE_GAMEPAD, MACRO_TYPE_BOTH};
        MACRO_TYPE type;
        bool all_modifiers = false;
        vector<Input::KeyboardButton> keyboard_buttons;
        vector<Input::GamepadButton> gamepad_buttons;

        Macro(vector<Input::KeyboardButton> keyboard_buttons);
        Macro(vector<Input::GamepadButton> gamepad_buttons);
        Macro(vector<Input::KeyboardButton> keyboard_buttons, vector<Input::GamepadButton> gamepad_buttons);
};

#endif
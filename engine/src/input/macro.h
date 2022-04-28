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

        Macro* setKb(vector<Input::KeyboardButton> buttons);
        Macro* setPad(vector<Input::GamepadButton> buttons);
        Macro* setMouse(vector<Input::MouseButton> buttons);

        template<typename T>
        bool isButtonSetPressed(vector<T> buttons);
        template<typename T>
        bool isButtonSetJustPressed(vector<T> buttons, bool all_are_modifiers);
        template<typename T>
        bool isButtonSetJustReleased(vector<T> buttons);
    
        static Macro* create();

    private:

        bool has_kb = false;
        bool has_pad = false;
        bool has_mouse = false;

        bool all_modifiers = false;
        vector<Input::KeyboardButton> keyboard_buttons;
        vector<Input::GamepadButton> gamepad_buttons;
        vector<Input::MouseButton> mouse_buttons;

};

#endif
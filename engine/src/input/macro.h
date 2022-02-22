#ifndef INCLUDED_MACRO
#define INCLUDED_MACRO

#include "engine/src/input/input_manager.h"

class Macro {
    public:
        Macro(vector<InputManager::KEYBOARD_BUTTON> keyboard_buttons);
        Macro(vector<InputManager::GAMEPAD_BUTTON> gamepad_buttons);
        Macro(vector<InputManager::KEYBOARD_BUTTON> keyboard_buttons, vector<InputManager::GAMEPAD_BUTTON> gamepad_buttons);

        bool isPressed();
        bool isJustPressed();
        bool isJustReleased();

        template<typename T>
        bool isButtonSetPressed(vector<T> buttons);
        template<typename T>
        bool isButtonSetJustPressed(vector<T> buttons, bool all_are_modifiers);
        template<typename T>
        bool isButtonSetJustReleased(vector<T> buttons);
    
    private:
        enum MACRO_TYPE {MACRO_TYPE_KEYBOARD, MACRO_TYPE_GAMEPAD, MACRO_TYPE_BOTH};
        MACRO_TYPE type;
        bool all_modifiers = false;
        vector<InputManager::KEYBOARD_BUTTON> keyboard_buttons;
        vector<InputManager::GAMEPAD_BUTTON> gamepad_buttons;
};

#endif
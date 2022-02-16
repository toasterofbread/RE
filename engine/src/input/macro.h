#include "engine/src/input/input.h"

class Macro {
    public:
        Macro(vector<KEYBOARD_BUTTON> buttons);
        Macro(vector<GAMEPAD_BUTTON> buttons);

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
        enum MACRO_TYPE {MACRO_TYPE_KEYBOARD, MACRO_TYPE_GAMEPAD};
        MACRO_TYPE type;
        bool all_modifiers = false;
        vector<KEYBOARD_BUTTON> keyboard_buttons;
        vector<GAMEPAD_BUTTON> gamepad_buttons;
};
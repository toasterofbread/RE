#ifndef INPUT_INCLUDED
#define INPUT_INCLUDED

#include "raylib-cpp.hpp"
#include <functional>
#include <vector>
#include <iostream>
using namespace std;

// Forward declarations
class InputEvent;
class NodeManager;
class Engine;

class InputManager {
    public:

        InputManager(Engine* engine_singleton);
        void init();

        enum GAMEPAD_BUTTON {
            PAD_LEFT = 4,
            PAD_RIGHT = 2,
            PAD_UP = 1,
            PAD_DOWN = 3,

            FACE_LEFT = 8,
            FACE_RIGHT = 6,
            FACE_UP = 5,
            FACE_DOWN = 7,

            STICK_CLICK_LEFT = 16,
            STICK_CLICK_RIGHT = 17,

            L1 = 9,
            R1 = 11,

            START = 15,
            SELECT = 13,
            HOME = 14,
        };

        enum KEYBOARD_BUTTON {
            ARROW_LEFT = 263,
            ARROW_RIGHT = 262,
            ARROW_UP = 265,
            ARROW_DOWN = 264,

            F1 = 290,
            F2 = 291,
            F3 = 292,
            F4 = 293,
            F5 = 294,
            F6 = 295,
            F7 = 296,
            F8 = 297,
            F9 = 298,
            F10 = 290,
            F11 = 300,
            F12 = 301,

            TAB = 258,
            LCTRL = 341,
            LALT = 342,
            LSHIFT = 340,
        };

        InputEvent* INPUTEVENT_PAD_UP;
        InputEvent* INPUTEVENT_PAD_DOWN;
        InputEvent* INPUTEVENT_PAD_LEFT;
        InputEvent* INPUTEVENT_PAD_RIGHT;

        InputEvent* INPUTEVENT_REBUILD_AND_RUN;

        static void process(float delta);
        static void printPressedKey();

        static bool isButtonPressed(GAMEPAD_BUTTON button);
        static bool isButtonJustPressed(GAMEPAD_BUTTON button);
        static bool isButtonJustReleased(GAMEPAD_BUTTON button);

        static bool isButtonPressed(KEYBOARD_BUTTON key);
        static bool isButtonJustPressed(KEYBOARD_BUTTON key);
        static bool isButtonJustReleased(KEYBOARD_BUTTON key);

        static bool isKeyModifier(KEYBOARD_BUTTON key);
        static bool isKeyModifier(GAMEPAD_BUTTON key);

        Vector2 getPadVector(bool just_pressed, float delta = 1.0f);

    private:
        int gamepad_id = 0;
        Engine* engine;
};

#endif

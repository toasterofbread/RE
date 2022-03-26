#ifndef INPUT_INCLUDED
#define INPUT_INCLUDED

#include "raylib-cpp.hpp"
#include <functional>
#include <vector>
#include <iostream>
using namespace std;

// Forward declarations
class InputEvent;
class SceneTree;
class Engine;

class InputManager {
    public:

        InputManager();
        static InputManager* getSingleton();

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

            KEY_A = 65,
            KEY_B = 66,
            KEY_C = 67,
            KEY_D = 68,
            KEY_E = 69,
            KEY_F = 70,
            KEY_G = 71,
            KEY_H = 72,
            KEY_I = 73,
            KEY_J = 74,
            KEY_K = 75,
            KEY_L = 76,
            KEY_M = 77,
            KEY_N = 78,
            KEY_O = 79,
            KEY_P = 80,
            KEY_Q = 81,
            KEY_R = 82,
            KEY_S = 83,
            KEY_T = 84,
            KEY_U = 85,
            KEY_V = 86,
            KEY_W = 87,
            KEY_X = 88,
            KEY_Y = 89,
            KEY_Z = 90,

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

            TILDE = 96,
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

        Vector2 getPadVector(float delta = 1.0f, bool just_pressed = false);

    private:
        static InputManager* singleton;
        
        int gamepad_id = 0;
};

#endif

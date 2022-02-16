#ifndef INPUT_INCLUDED
#define INPUT_INCLUDED

#include "raylib-cpp.hpp"

using namespace std;
#include <functional>
#include <vector>
#include <iostream>

const int gamepad_id = 0;

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

    TAB = 258,
    LCTRL = 341,
    LALT = 342,
    LSHIFT = 340,
};

void inputProcess(float delta);
void printPressedKey();

bool isButtonPressed(GAMEPAD_BUTTON button);
bool isButtonJustPressed(GAMEPAD_BUTTON button);
bool isButtonJustReleased(GAMEPAD_BUTTON button);

bool isButtonPressed(KEYBOARD_BUTTON key);
bool isButtonJustPressed(KEYBOARD_BUTTON key);
bool isButtonJustReleased(KEYBOARD_BUTTON key);

bool isKeyModifier(KEYBOARD_BUTTON key);
bool isKeyModifier(GAMEPAD_BUTTON key);

Vector2 getPadVector(bool just_pressed, float delta = 1.0f);

void addMacro(function<void(void)> func, vector<GAMEPAD_BUTTON> button_combination, string label = "", bool display_label = true);
void addMacro(function<void(void)> func, vector<KEYBOARD_BUTTON> key_combination, string label = "", bool display_label = true);

#endif

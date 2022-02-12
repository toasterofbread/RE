#include "raylib-cpp.hpp"

const int gamepad_id = 0;

enum GAMEPAD_BUTTONS {
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

enum KEYBOARD_BUTTONS {
    ARROW_LEFT = 263,
    ARROW_RIGHT = 262,
    ARROW_UP = 265,
    ARROW_DOWN = 264
};

bool isButtonPressed(int button);
bool isButtonJustPressed(int button);
bool isButtonJustReleased(int button);

bool isKeyPressed(int key);
bool isKeyJustPressed(int key);
bool isKeyJustReleased(int key);

Vector2 getPadVector(bool just_pressed, float delta = 1.0f);
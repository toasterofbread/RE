#ifndef INCLUDED_PLATFORM_DEFS
#define INCLUDED_PLATFORM_DEFS
#include "common/raylib.h"
#endif

#define TEXTURE_TYPE Texture2D

#define INPUT_HAS_PROCESS false

#define INPUT_DEFS \
enum Axis { \
    LX, \
    LY, \
    RX, \
    RY \
}; \
enum GamepadButton { \
    PAD_LEFT = GAMEPAD_BUTTON_LEFT_FACE_LEFT, \
    PAD_RIGHT = GAMEPAD_BUTTON_LEFT_FACE_RIGHT, \
    PAD_UP = GAMEPAD_BUTTON_LEFT_FACE_UP, \
    PAD_DOWN = GAMEPAD_BUTTON_LEFT_FACE_DOWN, \
    FACE_LEFT = GAMEPAD_BUTTON_RIGHT_FACE_LEFT, \
    FACE_RIGHT = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, \
    FACE_UP = GAMEPAD_BUTTON_RIGHT_FACE_UP, \
    FACE_DOWN = GAMEPAD_BUTTON_RIGHT_FACE_DOWN, \
    R1 = GAMEPAD_BUTTON_RIGHT_TRIGGER_1, \
    R2 = GAMEPAD_BUTTON_RIGHT_TRIGGER_2, \
    R3 = GAMEPAD_BUTTON_RIGHT_THUMB, \
    L1 = GAMEPAD_BUTTON_LEFT_TRIGGER_1, \
    L2 = GAMEPAD_BUTTON_LEFT_TRIGGER_2, \
    L3 = GAMEPAD_BUTTON_LEFT_THUMB, \
    START = GAMEPAD_BUTTON_MIDDLE_RIGHT, \
    SELECT = GAMEPAD_BUTTON_MIDDLE_LEFT, \
    HOME = GAMEPAD_BUTTON_MIDDLE, \
}; \
enum KeyboardButton { \
    KEY_ARROW_LEFT = 263, \
    KEY_ARROW_RIGHT = 262, \
    KEY_ARROW_UP = 265, \
    KEY_ARROW_DOWN = 264, \
    KEY_A = 65, \
    KEY_B = 66, \
    KEY_C = 67, \
    KEY_D = 68, \
    KEY_E = 69, \
    KEY_F = 70, \
    KEY_G = 71, \
    KEY_H = 72, \
    KEY_I = 73, \
    KEY_J = 74, \
    KEY_K = 75, \
    KEY_L = 76, \
    KEY_M = 77, \
    KEY_N = 78, \
    KEY_O = 79, \
    KEY_P = 80, \
    KEY_Q = 81, \
    KEY_R = 82, \
    KEY_S = 83, \
    KEY_T = 84, \
    KEY_U = 85, \
    KEY_V = 86, \
    KEY_W = 87, \
    KEY_X = 88, \
    KEY_Y = 89, \
    KEY_Z = 90, \
    KEY_F1 = 290, \
    KEY_F2 = 291, \
    KEY_F3 = 292, \
    KEY_F4 = 293, \
    KEY_F5 = 294, \
    KEY_F6 = 295, \
    KEY_F7 = 296, \
    KEY_F8 = 297, \
    KEY_F9 = 298, \
    KEY_F10 = 290, \
    KEY_F11 = 300, \
    KEY_F12 = 301, \
    KEY_TAB = 258, \
    KEY_LCTRL = 341, \
    KEY_LALT = 342, \
    KEY_LSHIFT = 340, \
    KEY_TILDE = 96, \
    KEY_ESC = 256, \
};

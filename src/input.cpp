#include "input.h"
#include "raylib-cpp.hpp"
#include "utils.h"

bool isButtonPressed(int button) {
    return IsGamepadButtonDown(gamepad_id, button);
}
bool isButtonJustPressed(int button){
    return IsGamepadButtonPressed(gamepad_id, button);
}
bool isButtonJustReleased(int button){
    return IsGamepadButtonReleased(gamepad_id, button);
}

bool isKeyPressed(int key) {
    return IsKeyDown(key);
}
bool isKeyJustPressed(int key) {
    return IsKeyPressed(key);
}
bool isKeyJustReleased(int key) {
    return IsKeyReleased(key);
}

Vector2 getPadVector(bool just_pressed, float delta) {
    Vector2 ret = Vector2{0, 0};

    if (just_pressed) {
        ret.x -= isButtonJustPressed(PAD_LEFT);
        ret.x += isButtonJustPressed(PAD_RIGHT);
        ret.y -= isButtonJustPressed(PAD_UP);
        ret.y += isButtonJustPressed(PAD_DOWN);

        ret.x -= isKeyJustPressed(ARROW_LEFT);
        ret.x += isKeyJustPressed(ARROW_RIGHT);
        ret.y -= isKeyJustPressed(ARROW_UP);
        ret.y += isKeyJustPressed(ARROW_DOWN);
    }
    else {
        ret.x -= isButtonPressed(PAD_LEFT);
        ret.x += isButtonPressed(PAD_RIGHT);
        ret.y -= isButtonPressed(PAD_UP);
        ret.y += isButtonPressed(PAD_DOWN);

        ret.x -= isKeyPressed(ARROW_LEFT);
        ret.x += isKeyPressed(ARROW_RIGHT);
        ret.y -= isKeyPressed(ARROW_UP);
        ret.y += isKeyPressed(ARROW_DOWN);
    }

    ret = sign(ret);

    return mV(ret, delta);
}
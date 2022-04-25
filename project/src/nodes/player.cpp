#include "player.h"

#include "common/draw.h"
#include "common/enums.h"
#include "common/input.h"
#include "node/types/camera_3d.h"

#define MOUSE_SENSITIVITY 1.5f


Camera3D* camera;

void Player::ready() {
    super::ready();
 
    camera = new Camera3D;
    addChild(camera);
    camera->enable();
}

void Player::process(float delta) {
    super::process(delta);

    Vector2 rotation = -Vector2(GetMouseDelta());
    if (rotation.isZero()) {
        rotation = Input::getAnalogPadVector(SIDE::RIGHT) * -1800.0f * delta;
    }
    setRotation((getRotation() + (Vector3(rotation.x, rotation.y, 0.0f) * MOUSE_SENSITIVITY * 0.001f * (camera->getCamera()->fovy / 45.0f))).clampAngle());
   
    Vector3 position = getPosition();

    Vector2 h_movement = Input::getPadVector();
    if (h_movement.isZero()) {
        h_movement = Input::getAnalogPadVector(SIDE::LEFT);
    }
    position = position.move(h_movement, getGlobalRotation(), 300.0f * delta);

    int v_movement = INPUT_EVENT_JUMP.isTriggered() - INPUT_EVENT_CROUCH.isTriggered();
    position.y += v_movement * delta * 5.0f;

    setPosition(position);

    camera->setZoom(min(max(camera->getZoom() + (GetMouseWheelMove() * -0.1f), 0.0f), 1.0f));
}
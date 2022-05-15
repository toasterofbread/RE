#ifndef INCLUDED_PLAYER
#define INCLUDED_PLAYER

#include "node/types/node_3d.h"

#include "node/types/camera_3d.h"
#include "input/input_event.h"
#include "input/macro.h"
#include "common/input.h"
#include "physics/node/physics_body_3d.h"

#include "world.h"
#include "entity.h"
#include "hotbar.h"

class Player: public Entity {

    public:

        REGISTER_NODE(Player, Entity, {});

        InputEvent INPUT_EVENT_JUMP = InputEvent({Macro::create()->setKb({Input::KEY_SPACE}), Macro::create()->setPad({Input::FACE_RIGHT})});
        InputEvent INPUT_EVENT_CROUCH = InputEvent({Macro::create()->setKb({Input::KEY_LSHIFT}), Macro::create()->setPad({Input::FACE_DOWN})});
        InputEvent INPUT_EVENT_INTERACT = InputEvent({Macro::create()->setMouse({Input::MOUSE_RIGHT}), Macro::create()->setPad({Input::L1})});
        InputEvent INPUT_EVENT_ATTACK = InputEvent({Macro::create()->setMouse({Input::MOUSE_LEFT}), Macro::create()->setPad({Input::R1})});
        InputEvent INPUT_EVENT_PICKBLOCK = InputEvent({Macro::create()->setMouse({Input::MOUSE_MIDDLE}), Macro::create()->setPad({Input::SELECT})});

        void ready();
        void process(float delta);
        void draw();

        Block* looking_at_block = NULL;
        DIRECTION_3 looking_at_face = DIRECTION_3::NONE;
    
    private:
        Vector2 camera_angle = Vector2(-PI/2, 0.0f);
        bool crouching = false;
        Hotbar hotbar;
        Camera3D camera;
        CollisionObject3D shape;

};

#endif
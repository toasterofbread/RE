#include "node/types/node_3d.h"

#include "input/input_event.h"
#include "input/macro.h"
#include "common/input.h"
#include "world.h"

class Player: public Node3D {

    public:

        REGISTER_NODE(Player, Node3D, {});

        InputEvent INPUT_EVENT_JUMP = InputEvent({Macro::create()->setKb({Input::KEY_SPACE}), Macro::create()->setPad({Input::FACE_RIGHT})});
        InputEvent INPUT_EVENT_CROUCH = InputEvent({Macro::create()->setKb({Input::KEY_LSHIFT}), Macro::create()->setPad({Input::FACE_DOWN})});
        InputEvent INPUT_EVENT_INTERACT = InputEvent({Macro::create()->setMouse({Input::MOUSE_RIGHT}), Macro::create()->setPad({Input::L1})});
        InputEvent INPUT_EVENT_ATTACK = InputEvent({Macro::create()->setMouse({Input::MOUSE_LEFT}), Macro::create()->setPad({Input::R1})});

        void ready();
        void process(float delta);
        void draw();

        World* world;
        Block* looking_at_block = NULL;
        DIRECTION_3 looking_at_face = DIRECTION_3::NONE;

};
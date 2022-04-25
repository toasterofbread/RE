#include "node/types/node_3d.h"

#include "input/input_event.h"
#include "input/macro.h"
#include "common/input.h"

class Player: public Node3D {

    public:

        REGISTER_NODE(Player, Node3D, {});

        InputEvent INPUT_EVENT_JUMP = InputEvent({Macro::create_kb({Input::KEY_SPACE}), Macro::create_pad({Input::FACE_RIGHT})});
        InputEvent INPUT_EVENT_CROUCH = InputEvent({Macro::create_kb({Input::KEY_LSHIFT}), Macro::create_pad({Input::FACE_DOWN})});

        void ready();
        void process(float delta);

};
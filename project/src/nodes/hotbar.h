#ifndef INCLUDED_HOTBAR
#define INCLUDED_HOTBAR

#include "node/types/node_2d.h"
#include "input/input_event.h"
#include "input/macro.h"
#include "engine.h"
#include "engine_texture.h"

#include "project/src/nodes/block.h"

#define HOTBAR_SIZE 9
#define HOTBAR_BOTTOM_MARGIN 25
#define HOTBAR_OPACITY 0.5f
#define HOTBAR_ICON_SCALE 0.5f

class Hotbar: public Node2D {

    public:

        REGISTER_NODE(Hotbar, Node2D, {});

        InputEvent INPUT_EVENT_SELECT_LEFT = InputEvent({Macro::create()->setMouse({Input::MOUSE_WHEEL_UP})});
        InputEvent INPUT_EVENT_SELECT_RIGHT = InputEvent({Macro::create()->setMouse({Input::MOUSE_WHEEL_DOWN})});

        void ready();
        void draw();

        void selectLeft();        
        void selectRight();

        Block::TYPE getSelected();
            
    private:
        void updatePosition();

        Block::TYPE available_blocks[HOTBAR_SIZE];
        int selected_item = 0;

        shared_ptr<EngineTexture> grid_texture = Engine::getSingleton()->loadTexture("project/resources/hotbar_grid.png");
        shared_ptr<EngineTexture> selection_texture = Engine::getSingleton()->loadTexture("project/resources/hotbar_selection.png");

};

#endif
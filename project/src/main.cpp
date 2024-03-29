#include "defs.h"
#include "engine/src/engine.h"
#include "common/utils.h"
#include "engine/src/node/scene_tree.h"
#include "engine/src/node/types/animated_sprite.h"
#include "engine/src/input/input_event.h"
#include "engine/src/core/resource/sprite_animation.h"
#include "node/types/camera_2d.h"
#include "node/types/camera_3d.h"
#include "node/scene_loader.h"
#include "input/macro.h"
#include "physics/physics_server.h"
#include "node/types/node_2d.h"

#include "project/src/nodes/player.h"
#include "project/src/nodes/world.h"
#include "project/src/nodes/hotbar.h"

#include "node/types/timer.h"

#include "project/src/nodes/block.h"

Engine* engine = Engine::getSingleton();

Macro* quit_macro = Macro::create()->setKb({Input::KEY_ESC});
Macro* tree_macro = Macro::create()->setKb({Input::KEY_TAB});
Macro* toggle_mouse_capture_macro = Macro::create()->setKb({Input::KEY_TILDE});

struct Project {

    Model plane;
    Model chonk;
    bool on = true;

    bool init() {
        Node* main_scene = SceneLoader::loadSceneFromFile("project/resources/main.rescn");
        engine->getTree()->addNode(main_scene);
        return true;
    }

    void toggle() {
        if (on) {
            for (Node* node : *engine->getTree()->getRoot()->getChildren()) {
                node->queueKill();
            }
            on = false;
        }
        else {
            init();
            on = true;
        }
    }

};

Project project;

bool should_close = false;
void mainLoop(float delta) {
    engine->process(delta);

    if (tree_macro->isJustPressed()) {
        engine->getTree()->getRoot()->printTree();
    }
    else if (quit_macro->isJustPressed()) {
        should_close = true;
    }
    else if (toggle_mouse_capture_macro->isJustPressed()) {
        if (IsCursorHidden()) {
            EnableCursor();
        }
        else {
            DisableCursor();
        }
    }
}

void dbPrint(string message, int column = 0, Colour colour = Colour::GREEN()) {
    Draw::drawText(message, Vector2(25, 30 + (column * 25)), colour);
}

void registerNodes();

int main() {

    OS::initialiseApplication();

    registerNodes();
    Engine::print_disabled = true;

    if (!project.init())
        return 1;

    while (!OS::shouldClose() && !Engine::fatal_error_occurred && !should_close) {
        
        float delta = OS::getFrameDelta();
        
        Draw::beginDrawing();

        mainLoop(delta);
        Draw::drawModel(project.plane, Vector3(0, 0, 0), 1.0f, Colour::WHITE());
        Draw::drawModel(project.chonk, Vector3(0, 0, 0), 1.0f, Colour::WHITE());

        int i = 0;
        dbPrint("Draw calls: " + to_string(Draw::getDrawCallCount()), i++);
        for (any message : *OS::getDbPrintStack()) {

            try {
                dbPrint(any_cast<string>(message), i);
                i++;
            }
            catch (bad_any_cast) {
                try {
                    float num = any_cast<float>(message);
                    dbPrint(to_string(num), i++, num >= 0.0f ? Colour::GREEN() : Colour::RED());
                }
                catch (bad_any_cast) {
                    dbPrint(any_cast<Vector3>(message).toString(), i++);
                }
            }
        }

        OS::resetDbPrint();
        OS::resetFrameStep();

        Draw::endDrawing();
    }

    while (Engine::fatal_error_occurred) {
        Draw::beginDrawing();
        dbPrint("Fatal error occurred: " + Engine::fatal_error);
        Draw::endDrawing();

        if (quit_macro->isJustPressed()) {
            OS::closeApplication();
            break;
        }
    }

    OS::closeApplication();
    return 1;
}

void registerNodes() {
    Player::registerNode();
    World::registerNode();
}

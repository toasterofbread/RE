#include "main.h"

#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>
#include <raylib-cpp.hpp>
#include "engine/src/input/input.h"
#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/core/node/animated_sprite.h"

#include "project/src/projectinit.h"

constexpr int WindowX = 960, WindowY = 540;

NodeManager* node_manager = new NodeManager;

void mainLoop() {
    float delta = GetFrameTime();
    node_manager->processNodes(delta);
    inputProcess(delta);
    projectLoop(delta);
}

int main() {
    
    ponderInit();

    SetTraceLogLevel(LOG_WARNING); // Disable info logs
    SetConfigFlags(FLAG_VSYNC_HINT); // Enable VSync
    InitWindow(WindowX, WindowY, "raylib test project"); // Create window
    SetWindowPosition(1920 + WindowX/2, WindowY/2); // Move window to center of second monitor
    SetWindowState(FLAG_WINDOW_RESIZABLE); // Make window resizable

    projectInit(node_manager);

    print(" --- Main loop started --- ");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{100, 100, 100, 255});
        mainLoop();
        DrawFPS(GetScreenWidth() - 85, 10);
        EndDrawing();
    }
    return 1;
}

PONDER_TYPE(Vector2)
PONDER_TYPE(NodeManager)
void ponderInit() {
    ponder::Class::declare<Vector2>("Vector2");
    ponder::Class::declare<NodeManager>("NodeManager");
    Node::declareSetters<Node>("Node");
    Node::getNodeBuilder<Node>("Node").constructor<NodeManager*>();
    AnimatedSprite::declareSetters<AnimatedSprite>("AnimatedSprite");
    Node::getNodeBuilder<AnimatedSprite>("AnimatedSprite").constructor<NodeManager*>();
}
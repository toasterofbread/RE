#include "raylib-cpp.hpp"
#include "utils.h"
#include "core/node_manager.h"
#include "core/nodes/animated_sprite.h"
#include "input.h"

constexpr int WindowX = 960, WindowY = 540;

NodeManager* manager = new NodeManager;
AnimatedSprite* sprite = new AnimatedSprite;

const float VELOCITY = 10.0f * 60.0f;

void mainLoop() {

    float delta = GetFrameTime();
    manager->processNodes(delta);

    Vector2 sprite_position = sprite->getPosition();
    sprite->setPosition(aV(sprite_position, mV(getPadVector(false, delta), VELOCITY)));

    return;
}

int main() {
    
    SetTraceLogLevel(LOG_WARNING); // Disable info logs
    SetConfigFlags(FLAG_VSYNC_HINT); // Enable VSync
    InitWindow(WindowX, WindowY, "raylib test project"); // Create window
    SetWindowPosition(1920 + WindowX/2, WindowY/2); // Move window to center of second monitor
    SetWindowState(FLAG_WINDOW_RESIZABLE); // Make window resizable

    sprite = new AnimatedSprite;
    manager->addNode(sprite);
    SpriteAnimationSet* animation_set = new SpriteAnimationSet(sprite);
    animation_set->loadFile("resources/sprite_animations/samus.json", "assets/sprites/samus/varia/");

    sprite->setAnimationSet(animation_set);
    sprite->play("powergrip_aim_down_left");

    // SpriteSheet* sprite_sheet = new SpriteSheet;
    // sprite_sheet->setTextureFile("assets/run_left.png", 10);

    // sprite->setSpriteSheet(sprite_sheet);
    // sprite->setFrameRate(30);
    // manager.addNode(sprite);

    print(" --- Main loop started --- ");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{100, 100, 100, 255});
        mainLoop();
        DrawFPS(GetScreenWidth() - 85, 10);
        EndDrawing();
    }
}
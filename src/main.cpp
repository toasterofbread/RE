#include "raylib-cpp.hpp"
#include "utils.h"
#include "engine/node_manager.h"
#include "engine/animated_sprite.h"

constexpr int WindowX = 960, WindowY = 540;

NodeManager manager;
AnimatedSprite* sprite;

const int VELOCITY = 10;
enum KEYS {
    ARROW_LEFT = 263,
    ARROW_RIGHT = 262,
    ARROW_UP = 265,
    ARROW_DOWN = 264
};

void mainLoop() {

    float delta = GetFrameTime();
    manager.processNodes(delta);

    Vector2 sprite_position = sprite->getPosition();
    
    if (IsKeyDown(ARROW_LEFT)) {
        sprite_position.x -= VELOCITY;
    }
    if (IsKeyDown(ARROW_RIGHT)) {
        sprite_position.x += VELOCITY;
    }
    if (IsKeyDown(ARROW_UP)) {
        sprite_position.y -= VELOCITY;
    }
    if (IsKeyDown(ARROW_DOWN)) {
        sprite_position.y += VELOCITY;
    }

    print(sprite_position);
    sprite->setPosition(sprite_position);

    return;
}

int main() {
    const Vector2 centerPos = Vector2{WindowX/2, WindowY/2};
    
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WindowX, WindowY, "raylib test project");
    SetWindowPosition(1920 + centerPos.x, centerPos.y); // Center of second monitor
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    SpriteSheet* sprite_sheet = new SpriteSheet;
    sprite_sheet->setTextureFile("../assets/run_left.png", 10);

    sprite = new AnimatedSprite;
    sprite->setSpriteSheet(sprite_sheet);
    sprite->setFrameRate(30);

    manager.addNode(sprite);
    
    // Node* node = new Node;
    // manager.addNode(node);

    print(" --- Main loop started --- ");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{100, 100, 100, 255});
        mainLoop();
        EndDrawing();
    }

    // while (!WindowShouldClose()) {
    //     timer += GetFrameTime();

    //     // print(currentFrame);



    //     EndDrawing();
    // }

}
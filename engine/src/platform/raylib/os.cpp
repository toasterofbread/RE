#include "engine/src/core/os.h"

#include "common/utils.h"
#include "engine.h"
#include "defs.h"

#include <iostream>
#include "common/raylib.h"
using namespace std;

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "UNDEFINED"
#endif

constexpr int WindowX = 960, WindowY = 540;

list<string> db_print_stack;
void OS::dbPrint(string message) {
    db_print_stack.push_back(message);
}
list<string>* OS::getDbPrintStack() { return &db_print_stack; }
void OS::resetDbPrint() {
    db_print_stack.clear();
}

void OS::initialiseApplication() {    
    SetTraceLogLevel(LOG_WARNING); // Disable info logs
    SetConfigFlags(FLAG_VSYNC_HINT); // Enable VSync
    InitWindow(WindowX, WindowY, ("RE Pre-Alpha Build #" + string(GIT_COMMIT_HASH)).c_str()); // Create window
    SetWindowPosition(1920 + WindowX/2, WindowY/2); // Move window to center of second monitor
    SetWindowState(FLAG_WINDOW_RESIZABLE); // Make window resizable
}

void OS::closeApplication() {

}

TEXTURE_TYPE OS::loadTexture(string path) {
    return LoadTexture(OS::getResPath(path).c_str());
}

void OS::unloadTexture(TEXTURE_TYPE texture) {
    UnloadTexture(texture);
}

TEXTURE_TYPE OS::generateTexture(unsigned int width, unsigned int height) {
    Image image = GenImageColor(width, height, Colour::WHITE());
    Texture2D ret = LoadTextureFromImage(image);
    UnloadImage(image);
    return ret;
}

int OS::getTextureWidth(TEXTURE_TYPE texture) {
    return texture.width;
}

int OS::getTextureHeight(TEXTURE_TYPE texture) {
    return texture.height;
}

Vector2 OS::getTextureSize(TEXTURE_TYPE texture) {
    return Vector2(texture.width, texture.height);
}

void OS::sleep(float seconds) {
    WaitTime(seconds * 1000);
}

void OS::beginDrawing() {
    BeginDrawing();
    ClearBackground(Colour{100, 100, 100, 255});
}

void OS::endDrawing() {
    EndDrawing();
}

string OS::getResPath(string path) {
    return plusFile("/home/spectre7/Projects/raylib/RE/", path);
}

double OS::getTime() {
    return GetTime();
}

int OS::getScreenWidth() {
    return GetScreenWidth();
}

int OS::getScreenHeight() {
    return GetScreenHeight();
}

bool OS::fileExists(string path) {
    return FileExists(OS::getResPath(path).c_str());
}

json OS::loadJsonFile(string path) {
    char* data = LoadFileText(OS::getResPath(path).c_str());
    json ret = json::parse(data);
    UnloadFileText(data);
    return ret;
}

bool OS::shouldClose() {
    return WindowShouldClose();
}

float OS::getFrameDelta() {
    return GetFrameTime();
}

void OS::print(string message, int type) {
    switch (type) {
        case 0: // Standard message
            cout << message << endl;
            break;
        case 1: // Important info
            cout << "\033[0;36;49m" << message << "\033[0m" << endl;
            break;
        case 2: // Warning
            cout << "\033[0;33;49m" << message << "\033[0m" << endl;
            break;
        case 3: // Error
            cout << "\033[0;30;41m" << message << "\033[0m" << endl;
            break;
    }
}
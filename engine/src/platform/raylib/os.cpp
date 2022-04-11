#include "engine/src/core/os.h"

#include <raylib-cpp.hpp>

#include "common/utils.h"

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "UNDEFINED"
#endif

constexpr int WindowX = 960, WindowY = 540;

void OS::initialiseApplication() {    

    SetTraceLogLevel(LOG_WARNING); // Disable info logs
    SetConfigFlags(FLAG_VSYNC_HINT); // Enable VSync
    InitWindow(WindowX, WindowY, concatChars("RaylibEngine Pre-Alpha Build #", GIT_COMMIT_HASH).c_str()); // Create window
    SetWindowPosition(1920 + WindowX/2, WindowY/2); // Move window to center of second monitor
    SetWindowState(FLAG_WINDOW_RESIZABLE); // Make window resizable

}

void OS::closeApplication() {

}

double OS::time() {
    return GetTime();
}
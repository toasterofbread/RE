// #ifndef INPUT_INCLUDED
// #define INPUT_INCLUDED

// #include "common/input.h"
// #include "common/vector2.h"
// #include "common/enums.h"
// #include "defs.h"

// #include <functional>
// #include <vector>
// #include <iostream>
// using namespace std;

// // Forward declarations
// class InputEvent;
// class SceneTree;
// class Engine;

// // !todo Remove class? Yes
// class InputManager {
//     public:

//         InputManager();
//         static InputManager* getSingleton();

//         InputEvent* INPUTEVENT_PAD_UP;
//         InputEvent* INPUTEVENT_PAD_DOWN;
//         InputEvent* INPUTEVENT_PAD_LEFT;
//         InputEvent* INPUTEVENT_PAD_RIGHT;

//         InputEvent* INPUTEVENT_REBUILD_AND_RUN;

//         static void printPressedKey();

//         static bool isKeyModifier(Input::KeyboardButton key);
//         static bool isKeyModifier(Input::GamepadButton key);

//         Vector2 getPadVector(bool just_pressed = false);
//         Vector2 getAnalogPadVector(SIDE side, float deadzone = 0.4f);

//     private:
//         static InputManager* singleton;
        
//         int gamepad_id = 0;
// };

// #endif

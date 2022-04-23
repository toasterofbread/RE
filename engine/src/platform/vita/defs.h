#include "core/os.h"
#include "common/utils.h"

#include "common/raylib.h"
#include <psp2/ctrl.h>
#include <string>
#include <list>
#include <unordered_map>
using namespace std;

struct VitaResource;
struct VitaTexture;
struct VitaTextureData;

#define TEXTURE_TYPE VitaTexture*

#define VITA_PARTITION ((string)"app0:")

#define INPUT_DEFS \
enum Axis { \
    LX, \
    LY, \
    RX, \
    RY \
}; \
enum GamepadButton { \
    PAD_LEFT = GAMEPAD_BUTTON_LEFT_FACE_LEFT, \
    PAD_RIGHT = GAMEPAD_BUTTON_LEFT_FACE_RIGHT, \
    PAD_UP = GAMEPAD_BUTTON_LEFT_FACE_UP, \
    PAD_DOWN = GAMEPAD_BUTTON_LEFT_FACE_DOWN, \
    FACE_LEFT = GAMEPAD_BUTTON_RIGHT_FACE_LEFT, \
    FACE_RIGHT = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, \
    FACE_UP = GAMEPAD_BUTTON_RIGHT_FACE_UP, \
    FACE_DOWN = GAMEPAD_BUTTON_RIGHT_FACE_DOWN, \
    R1 = GAMEPAD_BUTTON_RIGHT_TRIGGER_1, \
    R2 = GAMEPAD_BUTTON_RIGHT_TRIGGER_2, \
    R3 = GAMEPAD_BUTTON_RIGHT_THUMB, \
    L1 = GAMEPAD_BUTTON_LEFT_TRIGGER_1, \
    L2 = GAMEPAD_BUTTON_LEFT_TRIGGER_2, \
    L3 = GAMEPAD_BUTTON_LEFT_THUMB, \
    START = GAMEPAD_BUTTON_MIDDLE_RIGHT, \
    SELECT = GAMEPAD_BUTTON_MIDDLE_LEFT, \
    HOME = GAMEPAD_BUTTON_MIDDLE, \
}; \
enum KeyboardButton { \
    KEY_ARROW_LEFT, \
    KEY_ARROW_RIGHT, \
    KEY_ARROW_UP, \
    KEY_ARROW_DOWN, \
    KEY_A, \
    KEY_B, \
    KEY_C, \
    KEY_D, \
    KEY_E, \
    KEY_F, \
    KEY_G, \
    KEY_H, \
    KEY_I, \
    KEY_J, \
    KEY_K, \
    KEY_L, \
    KEY_M, \
    KEY_N, \
    KEY_O, \
    KEY_P, \
    KEY_Q, \
    KEY_R, \
    KEY_S, \
    KEY_T, \
    KEY_U, \
    KEY_V, \
    KEY_W, \
    KEY_X, \
    KEY_Y, \
    KEY_Z, \
    KEY_F1, \
    KEY_F2, \
    KEY_F3, \
    KEY_F4, \
    KEY_F5, \
    KEY_F6, \
    KEY_F7, \
    KEY_F8, \
    KEY_F9, \
    KEY_F10, \
    KEY_F11, \
    KEY_F12, \
    KEY_TAB, \
    KEY_LCTRL, \
    KEY_LALT, \
    KEY_LSHIFT, \
    KEY_TILDE, \
    KEY_ESC, \
}; \
enum MouseButton { \
    MOUSE_LEFT, \
    MOUSE_RIGHT, \
    MOUSE_MIDDLE, \
    MOUSE_SIDE, \
    MOUSE_EXTRA, \
    MOUSE_FORWARD, \
    MOUSE_BACK, \
    MOUSE_WHEEL_UP, \
    MOUSE_WHEEL_DOWN, \
};

#ifndef INCLUDED_PLATFORM_DEFS
#define INCLUDED_PLATFORM_DEFS

struct VitaTextureData {
    int x;
    int y;
    int w;
    int h;
    string path;
    string key_path;

    VitaTextureData(string texture_path, string texture_key_path, int x_pos, int y_pos, int width, int height) {
        path = texture_path;
        key_path = texture_key_path;
        x = x_pos;
        y = y_pos;
        w = width;
        h = height;
    }
    
    static VitaTextureData* createEmpty() {
        VitaTextureData* ret = new VitaTextureData;
        ret->x = 0;
        ret->y = 0;
        ret->w = 0;
        ret->h = 0;
        ret->path = "NONE";
        return ret;
    }

    private:
        VitaTextureData() {};
};

class VitaResource {
    public:
        enum class TYPE {
            FILE,
            TEXTURE
        };

        TYPE getType() { return type; }
        string getPath() { return resource_path; }

        VitaTextureData* getTextureData();

        static VitaResource* createFILE(string path, string key_path);
        static VitaResource* createTexture(string path, string key_path, int a, int b, int c, int d);

    static unordered_map<string, VitaResource*> index;
    
    private:
        string resource_path;
        string key_path;
        TYPE type;

        VitaResource(TYPE _type) { type = _type; }
        void setPath(string path);
        void setKeyPath(string _key_path);

        VitaTextureData* texture_data;
};

struct VitaTexture {
    static unordered_map<string, VitaTexture*> index;
    VitaTextureData* data;
    Texture2D texture;
    VitaTexture(Texture2D tex) {
        texture = tex;
    }
};

#endif
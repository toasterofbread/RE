#include "core/os.h"
#include "common/utils.h"

#include <vita2d.h>
#include <psp2/ctrl.h>
#include <string>
#include <list>
#include <unordered_map>
using namespace std;

struct VitaResource;
struct VitaTexture;
struct VitaTextureData;

#define TEXTURE_TYPE VitaTexture*

#define INPUT_HAS_PROCESS true

#define VITA_PARTITION ((string)"app0:")

#define INPUT_DEFS \
enum Axis { \
    LX, \
    LY, \
    RX, \
    RY \
}; \
enum GamepadButton { \
    PAD_LEFT = SCE_CTRL_LEFT, \
    PAD_RIGHT = SCE_CTRL_RIGHT, \
    PAD_UP = SCE_CTRL_UP, \
    PAD_DOWN = SCE_CTRL_DOWN, \
    FACE_LEFT = SCE_CTRL_SQUARE, \
    FACE_RIGHT = SCE_CTRL_CIRCLE, \
    FACE_UP = SCE_CTRL_TRIANGLE, \
    FACE_DOWN = SCE_CTRL_CROSS, \
    R1 = SCE_CTRL_RTRIGGER, \
    R2 = SCE_CTRL_R2, \
    R3 = SCE_CTRL_R3, \
    L1 = SCE_CTRL_LTRIGGER, \
    L2 = SCE_CTRL_L2, \
    L3 = SCE_CTRL_L3, \
    START = SCE_CTRL_START, \
    SELECT = SCE_CTRL_SELECT, \
    HOME = SCE_CTRL_PSBUTTON, \
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

    // bool isTextureLoaded() {
    //     return texture != NULL;
    // }

    // TEXTURE_TYPE getTexture() {
    //     return texture;
    // }

    // void setTexture(TEXTURE_TYPE texture) {
    //     this->texture = texture;
    // }

    private:
        VitaTextureData() {};
        // TEXTURE_TYPE texture = NULL;

};

class VitaResource {
    public:
        enum class TYPE {
            JSON,
            TEXTURE
        };

        TYPE getType() { return type; }
        string getPath() { return resource_path; }

        VitaTextureData* getTextureData() {
            assert(type == TYPE::TEXTURE);
            return texture_data;
        }

        static VitaResource* createJSON(string path, string key_path) {
            VitaResource* ret = new VitaResource(TYPE::JSON);
            ret->setPath(path);
            ret->setKeyPath(key_path);
            return ret;
        }
        static VitaResource* createTexture(string path, string key_path, int a, int b, int c, int d) {
            VitaResource* ret = new VitaResource(TYPE::TEXTURE);
            ret->setPath(path);
            ret->setKeyPath(key_path);
            ret->texture_data = new VitaTextureData(ret->resource_path, ret->key_path, a, b, c, d);
            return ret;
        }

    static unordered_map<string, VitaResource*> index;
    
    private:
        string resource_path;
        string key_path;
        TYPE type;

        VitaResource(TYPE _type) { type = _type; }
        void setPath(string path) { resource_path = VITA_PARTITION + path; }
        void setKeyPath(string _key_path) { key_path = formatPath(_key_path); }

        VitaTextureData* texture_data;
};

struct VitaTexture {
    static unordered_map<string, VitaTexture*> index;
    VitaTextureData* data;
    vita2d_texture* texture;
    VitaTexture(vita2d_texture* tex) {
        texture = tex;
    }
};

#endif
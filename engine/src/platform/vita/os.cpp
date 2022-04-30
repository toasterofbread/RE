#include "engine/src/core/os.h"

#include "common/utils.h"
#include "common/draw.h"
#include "engine.h"
#include "defs.h"
#include "common/raylib.h"

#include <psp2/kernel/processmgr.h>
#include <psp2/io/fcntl.h> 
#include <psp2/rtc.h>
#include <psp2/json.h>
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>
#include <pib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pib.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/gxm.h>
#include <psp2/ctrl.h>
#include <psp2/system_param.h> 

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "UNDEFINED"
#endif

// !todo Rewrite or remove VitaTexture system 

int _newlib_heap_size_user  = 300 * 1024 * 1024;
uint32_t sceLibcHeapSize = 16 * 1024 * 1024;

const int WindowX = 960;
const int WindowY = 544;

void OS::sleep(float seconds) {
    sceKernelDelayThread(seconds * 1000000);
}

bool net_initialised = false;
bool app_closed = false;
unordered_map<string, VitaResource*> VitaResource::index;
unordered_map<string, VitaTexture*> VitaTexture::index;

#define SERVER_URL "http://192.168.10.111:5000/"
#define MAX_MESSAGE_LENGTH 1000
void sendServerMessage(string message, const char* url = SERVER_URL);

Texture2D OS::getRaylibTexture(VitaTexture* texture) {
    return texture->texture;
}

VitaTexture* OS::loadTexture(string path) {

    Draw::loadStep("Loading textures...");

    path = formatPath(path);

    auto indexed = VitaTexture::index.find(path);
    if (indexed != VitaTexture::index.end()) {
        return indexed->second;
    }

    VitaResource* resource = VitaResource::index[path];
    VitaTexture* texture = new VitaTexture(LoadTexture(resource->getPath().c_str()));
    texture->data = resource->getTextureData();

    VitaTexture::index.insert(make_pair(path, texture));

    return texture;
}

void OS::unloadTexture(VitaTexture* texture) {
    Draw::loadStep("Unloading textures...", true);
    UnloadTexture(texture->texture);
    VitaTexture::index.erase(texture->data->key_path);
}

VitaTexture* OS::generateTexture(unsigned int width, unsigned int height) {
    Image image = GenImageColor(width, height, Colour::WHITE());
    VitaTexture* ret = new VitaTexture(LoadTextureFromImage(image));
    UnloadImage(image);

    ret->data = VitaTextureData::createEmpty();
    ret->data->w = width;
    ret->data->h = height;

    return ret;
}

int OS::getTextureWidth(VitaTexture* texture) {
    return texture->data->w;
}

int OS::getTextureHeight(VitaTexture* texture) {
    return texture->data->h;
}

Vector2 OS::getTextureSize(VitaTexture* texture) {
    return Vector2(texture->texture.width, texture->texture.height);
}

json loadJsonResource(VitaResource* resource) {
    ASSERT(resource->getType() == VitaResource::TYPE::FILE);

    char* data = LoadFileText(resource->getPath().c_str());
    json ret = json::parse(data);
    UnloadFileText(data);

    return ret;
}

void netInit() {

    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    
    SceNetInitParam param;
    int size = 1 * 1024 * 1024;
    
    param.memory = malloc(size);
    param.size = size;
    param.flags = 0;
    
    sceNetInit(&param);
    sceNetCtlInit();

    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleLoadModule(SCE_SYSMODULE_SSL);
    
    sceHttpInit(1 * 1024 * 1024);
    sceSslInit(1 * 1024 * 1024);

    net_initialised = true;
}

void netTerm() {

    if (!net_initialised) {
        return;
    }

    sceNetCtlTerm();
    sceNetTerm();
    
    sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
    sceSslTerm();
    sceHttpTerm();
    
    sceSysmoduleUnloadModule(SCE_SYSMODULE_SSL);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

void OS::initialiseApplication() {    

    // SetConfigFlags(FLAG_VSYNC_HINT); // Enable VSync
    pibInit(PIB_SHACCCG);

    debugNetInit("192.168.10.111", 18194, 3);
    sleep(1);

    InitWindow(WindowX, WindowY, "raylib [textures] example - texture loading and drawing"); // Create window

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);

    VitaResource* index_resource = VitaResource::createFILE("resource_index.json", "");
    json index_data = loadJsonResource(index_resource);
    delete index_resource;

    for (auto& [key, value] : index_data.items()) {
        string path = key;
        formatPath(path);

        VitaResource* resource;

        ASSERT_MSG(value.type() == json::value_t::string || value.type() == json::value_t::array, "Invalid resource index value type " + string(value.type_name()) + " at key " + key);

        if (value.type() == json::value_t::string) {
            resource = VitaResource::createFILE(value.get<string>(), path);
        }
        else {
            json data = value[1];
            int a = data[0].get<int>();
            int b = data[1].get<int>();
            int c = data[2].get<int>();
            int d = data[3].get<int>();

            resource = VitaResource::createTexture(value[0].get<string>(), path, a, b, c, d);
        }

        VitaResource::index.insert(make_pair<string, VitaResource*>((string)path, (VitaResource*)resource));
    }

    OS::print("RE started (commit hash: " + string(GIT_COMMIT_HASH) + ")", 1);
}

void OS::closeApplication() {
	netTerm();
    CloseWindow();
    app_closed = true;
}

double OS::getTime() {
    SceUInt64 tick = sceKernelGetProcessTimeWide(); // Microseconds
    return ((double)tick) / 1000000.0;
}

bool OS::fileExists(string path) {
    return FileExists(getResPath(path).c_str()) || VitaResource::index.count(formatPath(path));
}

int OS::getScreenWidth() {
    return 960;
}

int OS::getScreenHeight() {
    return 544;
}

string OS::getResPath(string path) {
    auto resource = VitaResource::index.find(formatPath(path));
    if (resource != VitaResource::index.end()) {
        return resource->second->getPath();
    }
    return VITA_PARTITION + path;
}

json OS::loadJsonFile(string path) {
    return loadJsonResource(VitaResource::index[formatPath(path)]);
}

char* OS::loadFileText(string path) {
    return LoadFileText(getResPath(path).c_str());
}

bool OS::shouldClose() {
    return app_closed;
}

float OS::getFrameDelta() {
    return GetFrameTime();
}

int urlencode(char *dest, const char *src) {
    int i;
    int len = 0;
    for (i = 0; src[i]; i++) {
        if (isalnum(src[i])) {
            *dest++ = src[i];
            len++;
        } else {
            sprintf(dest, "%%%02X", src[i]);
            dest += 3;
            len += 3;
        }
    }
    *dest++ = 0;
    return len;
}

void OS::print(string message, int type) {
    json package = json::object();

    package["type"] = type;
    package["content"] = message;

    debugNetUDPPrintf(package.dump().c_str());
}

void sendServerMessage(string message, const char* url) {

    if (!net_initialised) {
        netInit();
    }

	int tpl = sceHttpCreateTemplate("Message", 1, 1);
    int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);

    char encoded_message[MAX_MESSAGE_LENGTH];
    int length = urlencode(encoded_message, message.c_str());

    char post_data[9 + length];
    snprintf(post_data, 9 + length, "content=%s", encoded_message);

    int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_POST, url, sizeof(post_data));
    sceHttpAddRequestHeader(request, "Content-Type", "application/x-www-form-urlencoded", SCE_HTTP_HEADER_OVERWRITE);
    sceHttpSendRequest(request, post_data, sizeof(post_data));
}
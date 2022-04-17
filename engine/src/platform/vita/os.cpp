#include "engine/src/core/os.h"

#include "common/utils.h"
#include "common/draw.h"
#include "engine.h"
#include "defs.h"

#include <psp2/kernel/processmgr.h>
#include <psp2/io/fcntl.h> 
#include <psp2/rtc.h>
#include <vita2d.h>
#include <psp2/json.h>
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

list<string> db_print_stack;

list<string>* OS::getDbPrintStack() {
    return &db_print_stack;
}

void OS::dbPrint(string message) {
    db_print_stack.push_back(message);
}

void OS::resetDbPrint() {
    db_print_stack.clear();
}

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

VitaTexture* OS::loadTexture(string path) {

    Draw::loadStep("Loading textures...");

    path = formatPath(path);

    auto indexed = VitaTexture::index.find(path);
    if (indexed != VitaTexture::index.end()) {
        return indexed->second;
    }

    VitaResource* resource = VitaResource::index[path];
    VitaTexture* texture = new VitaTexture(vita2d_load_PNG_file(resource->getPath().c_str()));
    texture->data = resource->getTextureData();

    VitaTexture::index.insert(make_pair(path, texture));

    return texture;
}

void OS::unloadTexture(VitaTexture* texture) {
    // Draw::loadStep("Unloading textures...");
    // vita2d_free_texture(texture->texture);
    // VitaTexture::index.erase(texture->data->key_path);
}

VitaTexture* OS::generateTexture(unsigned int width, unsigned int height) {
    // !todo
    // VitaTexture* ret = (VitaTexture*)vita2d_create_empty_texture(width, height);
    // ret->data = VitaTextureData::createEmpty();
    return NULL;
}

int OS::getTextureWidth(VitaTexture* texture) {
    return vita2d_texture_get_width(texture->texture);
}

int OS::getTextureHeight(VitaTexture* texture) {
    return vita2d_texture_get_height(texture->texture);
}

Vector2 OS::getTextureSize(VitaTexture* texture) {
    return Vector2(vita2d_texture_get_width(texture->texture), vita2d_texture_get_height(texture->texture));
}

json loadJsonResource(VitaResource* resource) {
    assert(resource->getType() == VitaResource::TYPE::JSON);

    FILE* fd = fopen(resource->getPath().c_str(), "rb");
    if (!fd) {
        warn("Could not open JSON resource at path " + resource->getPath(), true);
        return json::object();
    }

    fseek(fd, 0, SEEK_END);
    int size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    char* buf = (char*)malloc(size);
    fread(buf, size, 1, fd);
    fclose(fd);

    string data = string(buf);
    free(buf);

    while (data.length() > size) {
        data.pop_back();
    }

    json ret = nlohmann::json::parse(data);
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

    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);

	netInit();

    VitaResource* index_resource = VitaResource::createJSON("resource_index.json", "");
    json index_data = loadJsonResource(index_resource);
    delete index_resource;

    for (auto& [key, value] : index_data.items()) {
        string path = key;
        formatPath(path);

        VitaResource* resource;

        if (value.type() == json::value_t::string) {
            resource = VitaResource::createJSON(value.get<string>(), path);
        }
        else if (value.type() == json::value_t::array) {
            json data = value[1];
            int a = data[0].get<int>();
            int b = data[1].get<int>();
            int c = data[2].get<int>();
            int d = data[3].get<int>();

            resource = VitaResource::createTexture(value[0].get<string>(), path, a, b, c, d);
        }
        else {
            warn("Invalid resource index value type " + (string)value.type_name() + " at key " + key, true);
        }

        VitaResource::index.insert(make_pair<string, VitaResource*>((string)path, (VitaResource*)resource));
    }

    sendServerMessage("RE started (commit hash: " + string(GIT_COMMIT_HASH) + ")");
}

void OS::closeApplication() {
	netTerm();

    vita2d_fini();
    sceKernelExitProcess(0);

    app_closed = true;
}

void OS::beginDrawing() {
    vita2d_start_drawing();
    vita2d_clear_screen();
}

void OS::endDrawing() {
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

double OS::getTime() {
    SceUInt64 tick = sceKernelGetProcessTimeWide(); // Microseconds
    return ((double)tick) / 1000000.0;
}

bool OS::fileExists(string path) {
    return VitaResource::index.count(formatPath(path));
    // SceUID file = sceIoOpen(getResPath(path).c_str(), SCE_O_RDONLY, 0777);
    // bool exists = file >= 0;
    // sceIoClose(file);
    // return exists;
}

int OS::getScreenWidth() {
    return 960;
}

int OS::getScreenHeight() {
    return 544;
}

string OS::getResPath(string path) {
    return path;
}

json OS::loadJsonFile(string path) {
    return loadJsonResource(VitaResource::index[formatPath(path)]);
}

bool OS::shouldClose() {
    return app_closed;
}

float OS::getFrameDelta() {
    return 0.016666667f;
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

void OS::print(string message) {
    sendServerMessage(message);
}

void sendServerMessage(string message, const char* url) {

    if (Engine::print_disabled) {
        return;
    }

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
    int header = sceHttpAddRequestHeader(request, "Content-Type", "application/x-www-form-urlencoded", SCE_HTTP_HEADER_OVERWRITE);
    // int handle = sceHttpSendRequest(request, post_data, sizeof(post_data));
    sceHttpSendRequest(request, post_data, sizeof(post_data));

	// string readData = "";
	// char data[16*1024];
	// long read = 0;

	// while ((read = sceHttpReadData(request, &data, sizeof(data)-1)) > 0) {
	// 	data[read] = '\0';
    //     readData.append(data);
	// }

    // warn((string)post_data + " | " + readData, true);

}
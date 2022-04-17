#ifndef INCLUDED_OS
#define INCLUDED_OS

#include "defs.h"
#include "common/vector2.h"

#include <string>
#include <list>
#include <any>
using namespace std;

#include <json.hpp>
using json = nlohmann::json;

namespace OS {

    // Called once on startup, before the main loop begins
    void initialiseApplication();

    // Closes the application
    void closeApplication();

    void beginDrawing();
    void endDrawing();

    // Returns time since application start in seconds
    double getTime();

    void setTextureLocked(string path, bool locked);
    bool isTextureLocked(string path);

    TEXTURE_TYPE loadTexture(string path);
    void unloadTexture(TEXTURE_TYPE texture);

    TEXTURE_TYPE generateTexture(unsigned int width, unsigned int height);

    int getTextureWidth(TEXTURE_TYPE texture);
    int getTextureHeight(TEXTURE_TYPE texture);
    Vector2 getTextureSize(TEXTURE_TYPE texture);

    bool fileExists(string path);

    int getScreenWidth();
    int getScreenHeight();

    string getResPath(string path);

    json loadJsonFile(string path);
    json loadJsonResource(string resource_path); // !temp

    bool shouldClose();

    float getFrameDelta();

    void print(string message);

    list<string>* getDbPrintStack();

    void dbPrint(string message);
    template<typename T>
    void dbPrint(T message) {
        dbPrint(to_string(message));
    }

    void resetDbPrint();

    void sleep(float seconds);
}

#endif
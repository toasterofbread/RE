#ifndef INCLUDED_OS
#define INCLUDED_OS

#include "defs.h"
#include "common/vector2.h"
#include "common/vector3.h"

#include <string>
#include <list>
#include <any>
using namespace std;

#include <json.hpp>
using json = nlohmann::json;

// Forward declarations
class Node;

namespace OS {

    // Called once on startup, before the main loop begins
    void initialiseApplication();

    // Closes the application
    void closeApplication();

    // Returns time since application start in seconds
    double getTime();

    void setTextureLocked(string path, bool locked);
    bool isTextureLocked(string path);

    Texture2D getRaylibTexture(TEXTURE_TYPE texture);

    TEXTURE_TYPE loadTexture(string path);
    void unloadTexture(TEXTURE_TYPE texture);

    TEXTURE_TYPE generateTexture(unsigned int width, unsigned int height);

    int getTextureWidth(TEXTURE_TYPE texture);
    int getTextureHeight(TEXTURE_TYPE texture);
    Vector2 getTextureSize(TEXTURE_TYPE texture);

    bool fileExists(string path);

    int getScreenWidth();
    int getScreenHeight();
    Vector2 getScreenSize();
    float getAspectRatio();

    string getResPath(string path);

    json loadJsonFile(string path);
    char* loadFileText(string path);

    bool shouldClose();

    float getFrameDelta();

    void print(string msg, int type = 0);
    void print(Vector2 msg, int type = 0);
    void print(Vector3 msg, int type = 0);
    void print(double msg, int type = 0);
    void print(float msg, int type = 0);
    void print(int msg, int type = 0);
    void print(Node* msg, int type = 0);

    list<any>* getDbPrintStack();
    void dbPrintAny(any message);
    void dbPrint(string message);

    void resetDbPrint();

    void frameStep(string name);
    void resetFrameStep();

    void sleep(float seconds);
}

#endif
#include "common/utils.h"

#include "common/raylib.h"
#include "common/draw.h"
#include "engine/src/node/node.h"

#include <string.h>
#include "cstring"
#include <stdio.h>
#include <box2d/box2d.h>

#if DEBUG_ENABLED && PLATFORM == PLATFORM_RAYLIB
#include "include/backward.hpp"
#endif

using namespace std;

void markPosition(Vector2 position, string text, Colour colour, float radius, float width) {
    Draw::drawLine(position - Vector2{radius, 0}, position + Vector2{radius, 0}, colour, false);
    Draw::drawLine(position - Vector2{0, radius}, position + Vector2{0, radius}, colour, false);
    Draw::drawText(text.c_str(), position.x + 5, position.y - 15, colour, 1.0f, false);
}

void markPosition(int layer, Vector2 position, string text, Colour colour, float radius, float width) {
    Draw::drawOnLayer(layer, true, Draw::drawLine, position - Vector2{radius, 0}, position + Vector2{radius, 0}, colour, false);
    Draw::drawOnLayer(layer, true, Draw::drawLine, position - Vector2{0, radius}, position + Vector2{0, radius}, colour, false);
    Draw::drawOnLayer(layer, true, Draw::drawText, text, position.x + 5, position.y - 15, colour, 1.5f, false);
}

float constrainAngle(float x) {
    x = fmod(x + DEG2RAD(180), DEG2RAD(360));
    if (x < 0)
        x += DEG2RAD(360);
    return x - DEG2RAD(180);
}

Vector2 dir2vector(DIRECTION direction) {
    switch (direction) {
        case DIRECTION::UP: return Vector2::UP();
        case DIRECTION::DOWN: return Vector2::DOWN();
        case DIRECTION::LEFT: return Vector2::LEFT();
        case DIRECTION::RIGHT: return Vector2::RIGHT();
    }
    return Vector2();
}
Vector3 dir2vector(DIRECTION_3 direction) {
    switch (direction) {
        case DIRECTION_3::UP: return Vector3::UP();
        case DIRECTION_3::DOWN: return Vector3::DOWN();
        case DIRECTION_3::LEFT: return Vector3::LEFT();
        case DIRECTION_3::RIGHT: return Vector3::RIGHT();
        case DIRECTION_3::FRONT: return Vector3::FRONT();
        case DIRECTION_3::BACK: return Vector3::BACK();
    }
    return Vector3();
}

Vector2 sign(Vector2 value) {
    return Vector2{sign(value.x), sign(value.y)};
}

bool stringBeginsWith(string str, string match) {

    if (str.length() == 0 || match.length() == 0 || match.length() > str.length()) {
        return false;
    }

    for (int i = 0; i < match.length(); i++) {
        if (str[i] != match[i]) {
            return false;
        }
    }

    return true;
}

vector<string> splitString(string str, char splitter) {
    vector<string> result;
    string current = ""; 
    for(int i = 0; i < str.size(); i++){
        if(str[i] == splitter){
            if(current != ""){
                result.push_back(current);
                current = "";
            } 
            continue;
        }
        current += str[i];
    }
    if(current.size() != 0)
        result.push_back(current);
    return result;
}


const char* plusFile(const char* path, const char* file_to_add) {
    string pathstr(path);
    if (pathstr.back() != '/') {
        pathstr.append("/");
    }
    string addstr(file_to_add);
    if (addstr.front() == '/') {
        pathstr.erase(0);
    }
    pathstr.append(addstr);

    char* ret = (char*)calloc(pathstr.length() + 1, sizeof(char));
    strcpy(ret, pathstr.c_str());

    return ret;
}
string plusFile(string path, string file_to_add) {
    if (path.back() == '/') {
        if (file_to_add.front() == '/') {
            path.pop_back();
        }
    }
    else if (file_to_add.front() != '/') {
        path.append("/");
    }
    path.append(file_to_add);
    return path;
}

string strVector2str(vector<string> vector, string splitter) {
    string ret;

    for (int i = 0; i < vector.size(); i++) {
        ret += vector[i];
        if (i + 1 < vector.size()) {
            ret += splitter;
        }
    }

    return ret;
}

string stringPadDecimals(string str, int max_decimals) {

    if (max_decimals < 0) {
        return str;
    }

    int decimal_point = str.find(".");
    if (decimal_point >= 0) {
        str.erase(decimal_point + max_decimals + 1, str.size() - decimal_point - max_decimals);
    }
    return str;
}

void print_stacktrace(int additional_skip) {
    #if DEBUG_ENABLED && PLATFORM == PLATFORM_RAYLIB
    using namespace backward;
    StackTrace st;

    st.load_here(9); //Limit the number of trace depth to 99
    st.skip_n_firsts(3 + additional_skip);//This will skip some backward internal function from the trace

    Printer p;
    p.snippet = true;
    p.object = true;
    p.address = true;
    p.print(st, stderr);
    #endif
}

string formatPath(string& path) {
    if (path.back() == '/') {
        path.erase(path.back());
    }
    if (path.front() != '/') {
        path = "/" + path;
    }
    return path;
}

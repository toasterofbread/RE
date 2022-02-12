#include <string.h>
#include "cstring"
#include <stdio.h>
#include <execinfo.h>

#include "utils.h"
#include "core/node.h"
#include "backward.hpp"

using namespace std;

Vector2 sign(Vector2 value) {
    return Vector2{sign(value.x), sign(value.y)};
}

const char* concatenateChars(const char* char1, const char* char2) { // Returns char1 + char2 as a new char*
    char* ret = (char*)calloc(strlen(char1) + strlen(char2) + 1, sizeof(char));
    strcpy(ret, char1);
    strcat(ret, char2);
    return ret;
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
const char* plusFile(const char* path, std::string file_to_add) {
    string pathstr(path);
    if (pathstr.back() != '/') {
        pathstr.append("/");
    }
    if (file_to_add.front() == '/') {
        pathstr.erase(0);
    }
    pathstr.append(file_to_add);

    char* ret = (char*)calloc(pathstr.length() + 1, sizeof(char));
    strcpy(ret, pathstr.c_str());

    return ret;
}
const char* getResPath(const char* absolute_path) {
    return plusFile("/home/spectre7/Projects/raylib/SSG/project/", absolute_path);
}

std::string vector2str(Vector2 value) {

    std::string s0{"Vector2{ "}, s1{", "}, s2{" }"};
    std::string sink;

    char bufferx[64];
    char buffery[64];
    sprintf(bufferx, "%f", value.x);
    sprintf(buffery, "%f", value.y);

    std::string bx{bufferx};
    std::string by{buffery};

    sink.reserve(s0.size() + s1.size() + s2.size() + 1 + bx.size() + by.size());
    sink += s0;
    sink += bx;
    sink += s1;
    sink += by;
    sink += s2;

    return sink;
}

const char* int2char(int value) {
    char* ret = (char*)calloc(100, sizeof(char));
    std::sprintf(ret, "%d", value);
    return ret;
}
const char* string2char(std::string value) {
    char* ret = (char*)calloc(value.length(), sizeof(char));
    strcpy(ret, value.c_str());
    return ret;
}

string char2string(const char* value) {
    return (string)value;
}

Vector2 aV(Vector2 a, Vector2 b) {
    return Vector2{a.x+b.x, a.y+b.y};
}
Vector2 sV(Vector2 a, Vector2 b) {
    return Vector2{a.x-b.x, a.y-b.y};
}
Vector2 mV(Vector2 a, Vector2 b) {
    return Vector2{a.x*b.x, a.y*b.y};
}
Vector2 mV(Vector2 a, float b) {
    return Vector2{a.x*b, a.y*b};
}
Vector2 mV(Vector2 a, int b) {
    return Vector2{a.x*b, a.y*b};
}
Vector2 dV(Vector2 a, Vector2 b) {
    return Vector2{a.x/b.x, a.y/b.y};
}
Vector2 dV(Vector2 a, float b) {
    return Vector2{a.x/b, a.y/b};
}
Vector2 dV(Vector2 a, int b) {
    return Vector2{a.x/b, a.y/b};
}

void print(const char* msg1, const char* msg2) {
    print(concatenateChars(msg1, msg2));
}
void printNode(Node* value) {
    print(concatenateChars(concatenateChars(value->getTypeName(), " | ID: "), int2char(value->getId())));
}
void print(Vector2 value) {
    std::cout << vector2str(value) << std::endl;
}

void print_trace(void) {
    char **strings;
    size_t i, size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    puts("");
    free(strings);
}

void stacker(int additional_skip) {
    using namespace backward;
    StackTrace st;

    st.load_here(9); //Limit the number of trace depth to 99
    st.skip_n_firsts(3 + additional_skip);//This will skip some backward internal function from the trace

    Printer p;
    p.snippet = true;
    p.object = true;
    p.address = true;
    p.print(st, stderr);
}

void warn(const char* msg1, const char* msg2) {
    print("WARNING:");
    print(msg1, msg2);
}
#include <iostream>
#include <stdexcept>
#include "raylib-cpp.hpp"

#include "core/node.h"

using namespace std;

template <typename Any>
float sign(Any value) {
    return (value > 0.0f) - (value < 0.0f);
}
Vector2 sign(Vector2 value);

const char* concatenateChars(const char* char1, const char* char2);
const char* plusFile(const char* path, const char* file_to_add);
const char* plusFile(const char* path, std::string file_to_add);
const char* getResPath(const char* absolute_path);

string vector2str(Vector2 value);
const char* int2char(int value);
const char* string2char(std::string value);
string char2string(const char* value);

Vector2 aV(Vector2 a, Vector2 b);
Vector2 sV(Vector2 a, Vector2 b);
Vector2 mV(Vector2 a, Vector2 b);
Vector2 mV(Vector2 a, float b);
Vector2 mV(Vector2 a, int b);
Vector2 dV(Vector2 a, Vector2 b);
Vector2 dV(Vector2 a, float b);
Vector2 dV(Vector2 a, int b);

void printNode(Node* value);
template <typename Any>
void print(Any msg) {
    cout << msg << endl; 
}
void print(const char* msg1, const char* msg2);
void print(Vector2 value);

void stacker(int additional_skip = 0);

template <typename Any>
void warn(Any msg, bool throw_error = false) {
    if (throw_error) {
        stacker(1);
        throw std::runtime_error(msg);
    }
    else {
        print("WARNING:");
        print(msg);
        // stacker(1);
    }
}


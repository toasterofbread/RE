#include "iostream"
#include "raylib-cpp.hpp"
#include "engine/node.h"

using namespace std;

string vector2str(Vector2 value);

Vector2 addVector2(Vector2 a, Vector2 b);
Vector2 subVector2(Vector2 a, Vector2 b);
Vector2 multVector2(Vector2 a, Vector2 b);
Vector2 divVector2(Vector2 a, Vector2 b);

template <typename Any>
void print(Any msg) {
    cout << msg << std::endl;
}

void print(Node value);
void print(Vector2 value);

template <typename Any>
void warn(Any msg) {
    print("WARNING:");
    print(msg);
}
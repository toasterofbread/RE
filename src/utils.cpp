#include "utils.h"
#include <string.h>
#include "engine/node.h"

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

Vector2 addVector2(Vector2 a, Vector2 b) {
    return Vector2{a.x+b.x, a.y+b.y};
}
Vector2 subVector2(Vector2 a, Vector2 b) {
    return Vector2{a.x-b.x, a.y-b.y};
}
Vector2 multVector2(Vector2 a, Vector2 b) {
    return Vector2{a.x*b.x, a.y*b.y};
}
Vector2 divVector2(Vector2 a, Vector2 b) {
    return Vector2{a.x/b.x, a.y/b.y};
}

// void print(Node value) {
    // TODO
// }
void print(Vector2 value) {
    std::cout << vector2str(value) << std::endl;
}
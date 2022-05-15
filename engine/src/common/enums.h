#ifndef INCLUDED_ENUMS
#define INCLUDED_ENUMS

#include "common/vector2.h"
#include "common/vector3.h"

#include <string>
using namespace std;

enum class DIRECTION {
    LEFT, RIGHT, UP, DOWN, NONE
};
#define DIRECTION_COUNT 4

enum class DIRECTION_3 {
    LEFT, RIGHT, UP, DOWN, FRONT, BACK, NONE
};
#define DIRECTION_3_COUNT 6

DIRECTION_3 convertDirection(DIRECTION base);
DIRECTION convertDirection(DIRECTION_3 base);
DIRECTION_3 convertDirection(Vector3 normal);
DIRECTION convertDirection(Vector2 normal);

DIRECTION_3 invertDirection(DIRECTION_3 base);

DIRECTION_3 relativeDirection(DIRECTION_3 base, DIRECTION_3 offset );

string directionToString(DIRECTION dir);
string directionToString(DIRECTION_3 dir);

DIRECTION stringToDirection(string str);
DIRECTION_3 stringToDirection3(string str);

enum class AXIS {
    X, Y, Z
};

enum class SIDE {
    LEFT, RIGHT
};

#endif
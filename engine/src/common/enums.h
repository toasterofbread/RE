#ifndef INCLUDED_ENUMS
#define INCLUDED_ENUMS

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

DIRECTION_3 invertDirection(DIRECTION_3 base);

DIRECTION_3 relativeDirection(DIRECTION_3 base, DIRECTION_3 offset );

enum class AXIS {
    X, Y, Z
};

enum class SIDE {
    LEFT, RIGHT
};

#endif
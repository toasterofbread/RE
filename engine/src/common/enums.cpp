#include "enums.h"

#include "common/utils.h"

DIRECTION_3 convertDirection(DIRECTION base) {
    if (base == DIRECTION::NONE) {
        return DIRECTION_3::NONE;
    }
    return (DIRECTION_3)base;
}

DIRECTION convertDirection(DIRECTION_3 base) {
    switch (base) {
        case DIRECTION_3::FRONT:
        case DIRECTION_3::BACK:
        case DIRECTION_3::NONE: return DIRECTION::NONE;
        default: return (DIRECTION)base;
    }
}

DIRECTION_3 convertDirection(Vector3 normal) {
    if (normal.x > 0)
        return DIRECTION_3::RIGHT;
    if (normal.x < 0)
        return DIRECTION_3::LEFT;
    
    if (normal.y > 0)
        return DIRECTION_3::UP;
    if (normal.y < 0)
        return DIRECTION_3::DOWN;
    
    if (normal.z > 0)
        return DIRECTION_3::BACK;
    if (normal.z < 0)
        return DIRECTION_3::FRONT;
    
    return DIRECTION_3::NONE;
}

DIRECTION convertDirection(Vector2 normal) {
    if (normal.x > 0)
        return DIRECTION::RIGHT;
    if (normal.x < 0)
        return DIRECTION::LEFT;
    
    if (normal.y > 0)
        return DIRECTION::DOWN;
    if (normal.y < 0)
        return DIRECTION::UP;
    
    return DIRECTION::NONE;
}

DIRECTION_3 invertDirection(DIRECTION_3 base) {
    switch (base) {
        case DIRECTION_3::FRONT: return DIRECTION_3::BACK;
        case DIRECTION_3::BACK: return DIRECTION_3::FRONT;
        case DIRECTION_3::LEFT: return DIRECTION_3::RIGHT;
        case DIRECTION_3::RIGHT: return DIRECTION_3::LEFT;
        case DIRECTION_3::UP: return DIRECTION_3::DOWN;
        case DIRECTION_3::DOWN: return DIRECTION_3::UP;
        default: return DIRECTION_3::NONE;
    }
}

DIRECTION_3 relativeDirection(DIRECTION_3 base, DIRECTION_3 offset ) {

    switch (offset) {
        case DIRECTION_3::FRONT: return base;
        case DIRECTION_3::BACK: return invertDirection(base);
        case DIRECTION_3::NONE: return DIRECTION_3::NONE;
    }

    switch (base) {
        case DIRECTION_3::LEFT: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::BACK;
            case DIRECTION_3::RIGHT: return DIRECTION_3::FRONT;
            case DIRECTION_3::UP: return DIRECTION_3::UP;
            case DIRECTION_3::DOWN: return DIRECTION_3::DOWN;
        }
        case DIRECTION_3::RIGHT: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::FRONT;
            case DIRECTION_3::RIGHT: return DIRECTION_3::BACK;
            case DIRECTION_3::UP: return DIRECTION_3::UP;
            case DIRECTION_3::DOWN: return DIRECTION_3::DOWN;
        }
        case DIRECTION_3::UP: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::LEFT;
            case DIRECTION_3::RIGHT: return DIRECTION_3::LEFT;
            case DIRECTION_3::UP: return DIRECTION_3::BACK;
            case DIRECTION_3::DOWN: return DIRECTION_3::FRONT;
        }
        case DIRECTION_3::DOWN: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::LEFT;
            case DIRECTION_3::RIGHT: return DIRECTION_3::RIGHT;
            case DIRECTION_3::UP: return DIRECTION_3::FRONT;
            case DIRECTION_3::DOWN: return DIRECTION_3::BACK;
        }
        case DIRECTION_3::FRONT: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::LEFT;
            case DIRECTION_3::RIGHT: return DIRECTION_3::RIGHT;
            case DIRECTION_3::UP: return DIRECTION_3::UP;
            case DIRECTION_3::DOWN: return DIRECTION_3::DOWN;
        }
        case DIRECTION_3::BACK: switch (offset) {
            case DIRECTION_3::LEFT: return DIRECTION_3::RIGHT;
            case DIRECTION_3::RIGHT: return DIRECTION_3::LEFT;
            case DIRECTION_3::UP: return DIRECTION_3::UP;
            case DIRECTION_3::DOWN: return DIRECTION_3::DOWN;
        }
    }

    return DIRECTION_3::NONE;
}

string directionToString(DIRECTION dir) {
    switch (dir) {
        case DIRECTION::UP: return "UP";
        case DIRECTION::DOWN: return "DOWN";
        case DIRECTION::LEFT: return "LEFT";
        case DIRECTION::RIGHT: return "RIGHT";
        default: return "NONE";
    }
}

string directionToString(DIRECTION_3 dir) {
    switch (dir) {
        case DIRECTION_3::UP: return "UP";
        case DIRECTION_3::DOWN: return "DOWN";
        case DIRECTION_3::LEFT: return "LEFT";
        case DIRECTION_3::RIGHT: return "RIGHT";
        case DIRECTION_3::FRONT: return "FRONT";
        case DIRECTION_3::BACK: return "BACK";
        default: return "NONE";
    }
}

DIRECTION stringToDirection(string str) {
    str = lowerString(str);

    if (str == "up" || str == "u") {
        return DIRECTION::UP;
    }
    else if (str == "down" || str == "d") {
        return DIRECTION::DOWN;
    }
    else if (str == "left" || str == "l") {
        return DIRECTION::LEFT;
    }
    else if (str == "right" || str == "r") {
        return DIRECTION::RIGHT;
    }
    else {
        return DIRECTION::NONE;
    }

}

DIRECTION_3 stringToDirection3(string str) {
    str = lowerString(str);

    DIRECTION dir = stringToDirection(str);
    if (dir != DIRECTION::NONE) {
        return (DIRECTION_3)dir;
    }

    if (str == "front" || str == "f") {
        return DIRECTION_3::FRONT;
    }
    else if (str == "back" || str == "b") {
        return DIRECTION_3::BACK;
    }
    else {
        return DIRECTION_3::NONE;
    }

}

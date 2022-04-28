#include "enums.h"

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
#ifndef INCLUDED_RAYLIB
// #define INCLUDED_RAYLIB

#ifdef Vector2
#define DEF_VECTOR2
#undef Vector2
#endif

#include "include/raylib/raylib.h"
#include "include/raylib/utils.h"

#undef LIGHTGRAY
#undef GRAY
#undef DARKGRAY
#undef YELLOW
#undef GOLD
#undef ORANGE
#undef PINK
#undef RED
#undef MAROON
#undef GREEN
#undef LIME
#undef DARKGREEN
#undef SKYBLUE
#undef BLUE
#undef DARKBLUE
#undef PURPLE
#undef VIOLET
#undef DARKPURPLE
#undef BEIGE
#undef BROWN
#undef DARKBROWN
#undef WHITE
#undef BLACK
#undef BLANK
#undef MAGENTA
#undef RAYWHITE

#ifdef DEF_VECTOR2
#undef Vector2
#define Vector2 InternalVector2
#endif

#endif
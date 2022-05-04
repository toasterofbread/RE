#ifndef INCLUDED_TRANSFORM
#define INCLUDED_TRANSFORM

#include "engine/compiler_settings.h"
#include "common/vector3.h"
#include "raylib/raymath.h"

#include "common/raylib.h"
#include <string>
using namespace std;

#include <btBulletCollisionCommon.h>

struct Basis: public btMatrix3x3 {

};

struct InternalTransform: btTransform {

    InternalTransform() {}

};

#ifdef Transform
#undef Transform
#endif
#define Transform InternalTransform

#endif
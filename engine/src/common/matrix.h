#ifndef INCLUDED_MATRIX
#define INCLUDED_MATRIX

#include "engine/compiler_settings.h"
#include "common/vector3.h"
#include "raylib/raymath.h"

#include "common/raylib.h"
#include <string>
using namespace std;

#if PHYSICS_3D_ENABLED
#include <ode/ode.h>
#endif

#define ITERATE_VALUES(code) \
float* value = &m0; \
int i = 0; \
code; \
value = &m4; \
i++; \
code; \
value = &m8; \
i++; \
code; \
value = &m12; \
i++; \
code; \
value = &m1; \
i++; \
code; \
value = &m5; \
i++; \
code; \
value = &m9; \
i++; \
code; \
value = &m13; \
i++; \
code; \
value = &m2; \
i++; \
code; \
value = &m6; \
i++; \
code; \
value = &m10; \
i++; \
code; \
value = &m14; \
i++; \
code; \
value = &m3; \
i++; \
code; \
value = &m7; \
i++; \
code; \
value = &m11; \
i++; \
code; \
value = &m15; \
i++; \
code;

struct InternalMatrix: public Matrix {

    InternalMatrix() {}
    InternalMatrix(Matrix matrix) {
        m0 = matrix.m0;
        m4 = matrix.m4;
        m8 = matrix.m8;
        m12 = matrix.m12;
        m1 = matrix.m1;
        m5 = matrix.m5;
        m9 = matrix.m9;
        m13 = matrix.m13;
        m2 = matrix.m2;
        m6 = matrix.m6;
        m10 = matrix.m10;
        m14 = matrix.m14;
        m3 = matrix.m3;
        m7 = matrix.m7;
        m11 = matrix.m11;
        m15 = matrix.m15;
    }
    InternalMatrix(const float* data) {
        ITERATE_VALUES(*value = data[i]);
    }

    void populateArray(float* data) {
        ITERATE_VALUES(data[i] = *value);
    }

    float operator[](const int index) {
        ITERATE_VALUES(
            if (i == index) {
                return *value;
            }
        )
        throw exception();
        return 0.0f;
    }

    bool operator==(InternalMatrix& other) {
        ITERATE_VALUES(
            if (*value != other[i]) {
                return false;
            }
        )
        return true;
    }

    // string toString(int max_decimals = 1) const;

    // #if PHYSICS_3D_ENABLED
    // #endif

};

#ifdef Matrix
#undef Matrix
#endif
#define Matrix InternalMatrix

#endif
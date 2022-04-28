#include "vector3.h"

#include "common/utils.h"

InternalVector3 InternalVector3::fromJson(json data) {
    ASSERT(data.is_array());
    ASSERT(data.size() == 3);
    ASSERT(data[0].is_number());
    ASSERT(data[1].is_number());
    ASSERT(data[2].is_number());

    Vector3 ret;

    ret.x = data[0].get<float>();
    ret.y = data[1].get<float>();
    ret.z = data[2].get<float>();

    return ret;
}

string InternalVector3::toString(int max_decimals) const {
    return "{ " + stringPadDecimals(to_string(x), max_decimals) + ", " + stringPadDecimals(to_string(y), max_decimals) + ", " + stringPadDecimals(to_string(z), max_decimals) + " }";
}

InternalVector3 InternalVector3::deg2rad() const {
    return InternalVector3(DEG2RAD(x), DEG2RAD(y), DEG2RAD(z));
}

InternalVector3 InternalVector3::rad2deg() const {
    return InternalVector3(RAD2DEG(x), RAD2DEG(y), RAD2DEG(z));
}

InternalVector3 InternalVector3::clampAngle() const {
    InternalVector3 ret = InternalVector3(x, y, z);
    if(ret.y > DEG2RAD(90.0f))
    {
        ret.y = DEG2RAD(89.9f);
    }
    else if(ret.y < DEG2RAD(-90.0f))
    {
        ret.y = DEG2RAD(-89.9f);
    }
    return ret;
}

InternalVector3 InternalVector3::move(Vector2 direction, InternalVector3 rotation, float delta, bool vertical) const {
    InternalVector3 ret = InternalVector3(x, y, z);
    
    direction.Normalize();

    ret.x += ((sinf(rotation.x) * direction.y -
                            sinf(rotation.x) * -direction.y -
                            cosf(rotation.x) * -direction.x +
                            cosf(rotation.x) * direction.x) *
                        delta);

    if (vertical) {
        ret.y += ((sinf(rotation.y) * -direction.y -
                                sinf(rotation.y) * direction.y) *
                            delta);
    }

    ret.z += ((cosf(rotation.x) * direction.y -
                            cosf(rotation.x) * -direction.y +
                            sinf(rotation.x) * -direction.x -
                            sinf(rotation.x) * direction.x) *
                        delta);

    return ret;
}

InternalVector3 InternalVector3::cAngle() const {
    return InternalVector3(constrainAngle(x), constrainAngle(y), constrainAngle(z));
}

void InternalVector3::normalise() {
    float length = sqrtf(x * x + y * y + z * z);
    if (length == 0.0f) {
        length = 1.0f;
    }
    
    float ilength = 1.0f / length;
    x *= ilength;
    y *= ilength;
    z *= ilength;
}

InternalVector3 InternalVector3::normalised() const {
    InternalVector3 ret = InternalVector3(x, y, z);
    ret.normalise();
    return ret;
}

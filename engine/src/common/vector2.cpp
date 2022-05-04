#include "vector2.h"

#include "common/utils.h"

InternalVector2 InternalVector2::fromJson(json data) {
    ASSERT(data.is_array());
    ASSERT(data.size() == 2);
    ASSERT(data[0].is_number());
    ASSERT(data[1].is_number());

    Vector2 ret;

    ret.x = data[0].get<float>();
    ret.y = data[1].get<float>();

    return ret;
}

string InternalVector2::toString(int max_decimals) {
    return "{ " + stringPadDecimals(to_string(x), max_decimals) + ", " + stringPadDecimals(to_string(y), max_decimals) + " }";
}
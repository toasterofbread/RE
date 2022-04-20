#include "vector2.h"

#include "common/utils.h"

InternalVector2::InternalVector2(json data) {
    ASSERT(data.is_array());
    ASSERT(data.size() == 2);
    ASSERT(data[0].is_number());
    ASSERT(data[1].is_number());

    x = data[0].get<float>();
    y = data[1].get<float>();
}
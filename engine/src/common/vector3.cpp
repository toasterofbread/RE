#include "vector3.h"

#include "common/utils.h"

InternalVector3::InternalVector3(json data) {
    ASSERT(data.is_array());
    ASSERT(data.size() == 3);
    ASSERT(data[0].is_number());
    ASSERT(data[1].is_number());
    ASSERT(data[2].is_number());

    x = data[0].get<float>();
    y = data[1].get<float>();
    z = data[2].get<float>();
}
#pragma once

#include "Math.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdint>
#include "glad/glad.h"

using EntityID = int32_t;
const int32_t INVALID_ENTITY = -1;






struct Rotator{
    glm::vec3 axis;
};

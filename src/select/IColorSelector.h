#pragma once

#include "glm/ext/vector_float3.hpp"

class IColorSelector {
public:
    virtual glm::vec3 select(glm::vec3& target) = 0;
};

#pragma once

#include "glm/ext/vector_float3.hpp"
#include <functional>

class IColorSpace {
public:
    IColorSpace() {}
    virtual ~IColorSpace() {}

    virtual glm::vec3 fromRGB(glm::vec3 color) const = 0;
};

#define DEF_COLORSPACE \
    glm::vec3 fromRGB(glm::vec3 color) const override;


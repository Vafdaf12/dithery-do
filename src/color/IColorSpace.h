#pragma once

#include "glm/ext/vector_float3.hpp"

class IColorSpace {
public:
    IColorSpace() {}
    virtual ~IColorSpace() {}

    virtual glm::vec3 fromRGB(glm::vec3 value) const = 0;
    virtual glm::vec3 toRGB(glm::vec3 value) const = 0;
};

#define DEF_COLORSPACE                                                                             \
    glm::vec3 fromRGB(glm::vec3 color) const override;                                             \
    glm::vec3 toRGB(glm::vec3 color) const override;

#pragma once

#include "IColorSpace.h"
#include "color/XyzColorSpace.h"
#include "glm/ext/vector_float3.hpp"

class LabColorSpace : public IColorSpace {
public:
    DEF_COLORSPACE

    static const glm::vec3 D65;
    static const glm::vec3 D50;

    LabColorSpace(const glm::vec3& illuminant) : m_illuminant(illuminant) {}

private:
    static float func(float t);
    static float funcInv(float t);

    XyzColorSpace m_xyz;

    static constexpr double DELTA = 6/29.f;

    glm::vec3 m_illuminant;
};

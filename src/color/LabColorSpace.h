#pragma once

#include "IColorSpace.h"
#include "glm/ext/vector_float3.hpp"

class LabColorSpace : public IColorSpace {
public:
    static const glm::vec3 D65;
    static const glm::vec3 D50;

    LabColorSpace(const glm::vec3& illuminant) : m_illuminant(illuminant) {}
    glm::vec3 fromRGB(glm::vec3 color) const override;

private:
    static float func(float t);
    static glm::vec3 toXYZ(glm::vec3 col);

    static constexpr float DELTA = 6/29.f;

    glm::vec3 m_illuminant;
};

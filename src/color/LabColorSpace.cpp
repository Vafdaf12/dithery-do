#include "LabColorSpace.h"
#include "glm/ext/vector_float3.hpp"

#include <cmath>

const glm::vec3 LabColorSpace::D65(95.0489, 100, 108.884);
const glm::vec3 LabColorSpace::D50(96.4212, 100, 82.5188);

glm::vec3 LabColorSpace::fromRGB(glm::vec3 color) const {
    color = m_xyz.fromRGB(color);
    color /= m_illuminant;

    glm::vec3 lab;

    lab.x = 116 * func(color.y) - 16;
    lab.y = 500 * (func(color.x) - func(color.y));
    lab.z = 200 * (func(color.y) - func(color.z));

    return lab;
}
glm::vec3 LabColorSpace::toRGB(glm::vec3 color) const {
    glm::vec3 xyz; 
    xyz.x = funcInv((color.x + 16)/116 + color.y/500);
    xyz.y = funcInv((color.x + 16)/116);
    xyz.z = funcInv((color.x + 16)/116 - color.z/200);

    xyz *= m_illuminant;

    return m_xyz.toRGB(xyz);
}

float LabColorSpace::func(float t) {
    if (t > std::pow(DELTA, 3)) {
        return std::cbrt(t);
    } else {
        return t / (3 * DELTA * DELTA) + 4 / 29.f;
    }
}
float LabColorSpace::funcInv(float t) {
    if(t > DELTA) {
        return std::pow(t, 3);
    } else {
        return 3*DELTA*DELTA * (t - 4/29.0);
    }
}

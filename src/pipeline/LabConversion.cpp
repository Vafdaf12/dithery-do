#include "LabConversion.h"
#include <cmath>

const glm::vec3 LabConversion::D65(95.0489, 100, 108.884);
const glm::vec3 LabConversion::D50(96.4212, 100, 82.5188);

glm::vec3 LabConversion::process(glm::vec3 in) {
    if(m_toColor) {
        return toXyz(in);
    } else {
        return fromXyz(in);
    }
}

glm::vec3 LabConversion::fromXyz(glm::vec3 color) const {
    color /= m_illuminant;

    glm::vec3 lab;

    lab.x = 116 * func(color.y) - 16;
    lab.y = 500 * (func(color.x) - func(color.y));
    lab.z = 200 * (func(color.y) - func(color.z));

    return lab;
}
glm::vec3 LabConversion::toXyz(glm::vec3 color) const {
    glm::vec3 xyz; 
    xyz.x = funcInv((color.x + 16)/116 + color.y/500);
    xyz.y = funcInv((color.x + 16)/116);
    xyz.z = funcInv((color.x + 16)/116 - color.z/200);

    return xyz * m_illuminant;
}
float LabConversion::func(float t) {
    if (t > std::pow(DELTA, 3)) {
        return std::cbrt(t);
    } else {
        return t / (3 * DELTA * DELTA) + 4 / 29.f;
    }
}
float LabConversion::funcInv(float t) {
    if(t > DELTA) {
        return std::pow(t, 3);
    } else {
        return 3*DELTA*DELTA * (t - 4/29.0);
    }
}

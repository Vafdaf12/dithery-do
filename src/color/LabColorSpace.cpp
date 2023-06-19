#include "LabColorSpace.h"

#include <cmath>

const glm::vec3 LabColorSpace::D65(95.0489,100, 108.884);
const glm::vec3 LabColorSpace::D50(96.4212, 100, 82.5188);

glm::vec3 LabColorSpace::fromRGB(glm::vec3 color) const {
    color = toXYZ(color);
    color = color / m_illuminant;
    glm::vec3 lab;

    lab.x = 116 * func(color.y) - 16;
    lab.y = 500 * (func(color.x) - func(color.y));
    lab.z = 200 * (func(color.y) - func(color.z));

    return lab;
}

float LabColorSpace::func(float t) {
    if(t > std::pow(DELTA, 3)) {
        return std::cbrt(t);
    }
    else {
        return t / (3 * DELTA*DELTA) + 4/29.f;
    }
}
glm::vec3 LabColorSpace::toXYZ(glm::vec3 color) {
    color.r = ((color.r > 0.04045) ? pow((color.r + 0.055) / 1.055, 2.4) : (color.r / 12.92)) * 100.0;
    color.g = ((color.g > 0.04045) ? pow((color.g + 0.055) / 1.055, 2.4) : (color.g / 12.92)) * 100.0;
    color.b = ((color.b > 0.04045) ? pow((color.b + 0.055) / 1.055, 2.4) : (color.b / 12.92)) * 100.0;

    color.r = color.r*0.4124564 + color.g*0.3575761 + color.b*0.1804375;
    color.g = color.r*0.2126729 + color.g*0.7151522 + color.b*0.0721750;
    color.b = color.r*0.0193339 + color.g*0.1191920 + color.b*0.9503041;
    return color;
}

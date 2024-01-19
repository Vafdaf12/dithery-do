#include "XyzColorSpace.h"

#include <cmath>

glm::vec3 XyzColorSpace::fromRGB(glm::vec3 color) const {
    color.r = ((color.r > 0.04045) ? std::pow((color.r + 0.055) / 1.055, 2.4) : (color.r / 12.92)) * 100.0;
    color.g = ((color.g > 0.04045) ? std::pow((color.g + 0.055) / 1.055, 2.4) : (color.g / 12.92)) * 100.0;
    color.b = ((color.b > 0.04045) ? std::pow((color.b + 0.055) / 1.055, 2.4) : (color.b / 12.92)) * 100.0;

    color.x = color.r*0.4124564 + color.g*0.3575761 + color.b*0.1804375;
    color.y = color.r*0.2126729 + color.g*0.7151522 + color.b*0.0721750;
    color.z = color.r*0.0193339 + color.g*0.1191920 + color.b*0.9503041;
    return color;
}
glm::vec3 XyzColorSpace::toRGB(glm::vec3 color) const {
    color /= 100.0f;

    color.r = color.x * 3.2404542 + color.y * -1.5371385 + color.z * -0.4985314;
    color.g = color.x * -0.9692660 + color.y * 1.8760108 + color.z * 0.0415560;
    color.b = color.x * 0.0556434 + color.y * -0.2040259 + color.z * 1.0572252;

    color.r = ((color.r > 0.0031308) ? (1.055*pow(color.r, 1 / 2.4) - 0.055) : (12.92*color.r));
    color.g = ((color.g > 0.0031308) ? (1.055*pow(color.g, 1 / 2.4) - 0.055) : (12.92*color.g));
    color.b = ((color.b > 0.0031308) ? (1.055*pow(color.b, 1 / 2.4) - 0.055) : (12.92*color.b));

    return color;
}

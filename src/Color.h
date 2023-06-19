#pragma once

#include "glm/ext/vector_float3.hpp"
#include <concepts>
#include <math.h>

template<typename T>
concept Color = requires(T a, const glm::vec3& rgb) {
    { T::fromRGB(rgb) } -> std::convertible_to<glm::vec3>;
};


#define COLOR(type, body) struct type { body };
COLOR(Xyz, 
    static glm::vec3 fromRGB(glm::vec3 color) {
        color.r = ((color.r > 0.04045) ? pow((color.r + 0.055) / 1.055, 2.4) : (color.r / 12.92)) * 100.0;
        color.g = ((color.g > 0.04045) ? pow((color.g + 0.055) / 1.055, 2.4) : (color.g / 12.92)) * 100.0;
        color.b = ((color.b > 0.04045) ? pow((color.b + 0.055) / 1.055, 2.4) : (color.b / 12.92)) * 100.0;

        color.r = color.r*0.4124564 + color.g*0.3575761 + color.b*0.1804375;
        color.g = color.r*0.2126729 + color.g*0.7151522 + color.b*0.0721750;
        color.b = color.r*0.0193339 + color.g*0.1191920 + color.b*0.9503041;
        return color;
    }
)

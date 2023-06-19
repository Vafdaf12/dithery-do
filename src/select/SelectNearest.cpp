#include "SelectNearest.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <math.h>

SelectNearest::SelectNearest(const Palette& palette, bool xyz)
    : m_palette(palette)
    , m_xyz(xyz) {
}

glm::vec3 SelectNearest::toXYZ(glm::vec3 color) {
    color.r = ((color.r > 0.04045) ? pow((color.r + 0.055) / 1.055, 2.4) : (color.r / 12.92)) * 100.0;
    color.g = ((color.g > 0.04045) ? pow((color.g + 0.055) / 1.055, 2.4) : (color.g / 12.92)) * 100.0;
    color.b = ((color.b > 0.04045) ? pow((color.b + 0.055) / 1.055, 2.4) : (color.b / 12.92)) * 100.0;

    color.r = color.r*0.4124564 + color.g*0.3575761 + color.b*0.1804375;
    color.g = color.r*0.2126729 + color.g*0.7151522 + color.b*0.0721750;
    color.b = color.r*0.0193339 + color.g*0.1191920 + color.b*0.9503041;
    return color;
}

glm::vec3 SelectNearest::select(glm::vec3& target) {
    std::vector<glm::vec3> colors = m_palette.colors();
    glm::vec3 dest = target;
    if(m_xyz) {
        dest = toXYZ(dest);
        std::transform(colors.begin(), colors.end(), colors.begin(), toXYZ);
    }

    int i = std::max_element(
        colors.begin(),
        colors.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - colors.begin();

    return m_palette.colors()[i];
}

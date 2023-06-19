#include "SelectNearest.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <math.h>

SelectNearest::SelectNearest(const Palette& palette, IColorSpace* colorSpace)
    : m_palette(palette)
    , m_mappedColours(palette.colors())
    , m_pColorSpace(colorSpace)
{
    if(colorSpace) {
        std::transform(
            m_mappedColours.begin(),
            m_mappedColours.end(),
            m_mappedColours.begin(),
            [=](const glm::vec3& c) {
                return colorSpace->fromRGB(c);
            }
        );
    }
}

glm::vec3 SelectNearest::select(glm::vec3& target) {
    glm::vec3 dest = target;
    if(m_pColorSpace) dest = m_pColorSpace->fromRGB(dest);

    int i = std::max_element(
        m_mappedColours.begin(),
        m_mappedColours.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - m_mappedColours.begin();

    return m_palette.colors()[i];
}

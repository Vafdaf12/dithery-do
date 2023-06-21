#include "ClosestMix.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <math.h>

ClosestMix::ClosestMix(const Palette& palette, IColorSpace* colorSpace)
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

glm::vec3 ClosestMix::mapRGB(glm::vec3 col) const {
    if(!m_pColorSpace) return col;
    return m_pColorSpace->fromRGB(col);
}


glm::vec3 ClosestMix::select(glm::vec3& target) {
    glm::vec3 dest = mapRGB(target);

    int i = std::max_element(
        m_mappedColours.begin(),
        m_mappedColours.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - m_mappedColours.begin();

    glm::vec3 col1 = m_mappedColours[i];
    glm::vec3 dir = glm::normalize(col1-dest);

    int j = std::max_element(
        m_mappedColours.begin(),
        m_mappedColours.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            if(glm::dot(v - col1, dir) > 0) return false; 
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - m_mappedColours.begin();

    if(i == j) return m_palette.colors()[i];

    glm::vec3 col2 = m_mappedColours[j];
    dir = glm::normalize(col2 - col1);
    float t = glm::dot(dir, dest - col1) / glm::distance(col1, col2);

    return glm::mix(m_palette.colors()[i], m_palette.colors()[j], t);
}

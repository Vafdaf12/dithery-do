#include "ClosestPartition.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <math.h>
#include <numeric>

ClosestPartition::ClosestPartition(const Palette& palette, IColorSpace* colorSpace)
    : m_palette(palette)
    , m_mappedColours(palette.colors())
    , m_pColorSpace(colorSpace)
    , m_indexes(m_mappedColours.size())
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
    std::iota(m_indexes.begin(), m_indexes.end(), 0);
}

glm::vec3 ClosestPartition::mapRGB(glm::vec3 col) const {
    if(!m_pColorSpace) return col;
    return m_pColorSpace->fromRGB(col);
}


glm::vec3 ClosestPartition::get(int i) const {
    return m_palette.colors()[m_indexes[i]];
}

glm::vec3 ClosestPartition::select(glm::vec3& target) {
    std::vector<glm::vec3>& colors = m_mappedColours;
    std::vector<int>& indexes = m_indexes;

    glm::vec3 dest = mapRGB(target);

    const int max = colors.size()-1;
    const int min = 0;


    int closest = std::max_element(
        m_mappedColours.begin(),
        m_mappedColours.end(),
        [&](const glm::vec3& v, const glm::vec3& min) {
            return glm::distance(dest, v) > glm::distance(dest, min);
        }) - colors.begin();
    glm::vec3 dir = m_mappedColours[closest] - dest;

    // partition palette by brightness
    int upper = colors.size()-1;
    int lower = 0;
    while(lower <= upper) {
        while(glm::dot(colors[lower] - dest, dir) < 0 && lower <= max) lower++;
        while(glm::dot(colors[upper] - dest, dir) >= 0 && upper >= min) upper--;

        if(lower < upper) {
            std::swap(colors[lower], colors[upper]);
            std::swap(indexes[lower], indexes[upper]);
        }
    }
    int i = -1, j = -1;

    // select color from each partition
    if(upper > 0) i = std::max_element(
        colors.begin(),
        colors.begin()+upper,
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - colors.begin();

    if(upper <= colors.size()-1) j = std::max_element(
        colors.begin()+upper,
        colors.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - colors.begin();
    
    if(i == -1 || j == -1) return get(closest);

    // blend between colors
    glm::vec3 darker = colors[i];
    glm::vec3 lighter = colors[j];

    dir = glm::normalize(lighter - darker);
    float t = glm::dot(dest - darker, dir) / glm::distance(lighter, darker);

    return glm::mix(get(i), get(j), t);
}

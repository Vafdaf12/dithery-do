#include "BrightnessPartition.h"

#include "glm/geometric.hpp"
#include <algorithm>
#include <iterator>
#include <math.h>
#include <numeric>

BrightnessPartition::BrightnessPartition(const Palette& palette)
    : m_palette(palette)
    , m_mappedColours(palette.colors())
    , m_colorSpace(LabColorSpace::D65)
    , m_indexes(m_mappedColours.size())
    
{
    std::transform(
        m_mappedColours.begin(),
        m_mappedColours.end(),
        m_mappedColours.begin(),
        [=](const glm::vec3& c) {
            return m_colorSpace.fromRGB(c);
        }
    );
    std::iota(m_indexes.begin(), m_indexes.end(), 0);
}

glm::vec3 BrightnessPartition::mapRGB(glm::vec3 col) const {
    return m_colorSpace.fromRGB(col);
}


glm::vec3 BrightnessPartition::get(int i) const {
    return m_palette.colors()[m_indexes[i]];
}
glm::vec3 BrightnessPartition::select(glm::vec3& target) {
    std::vector<glm::vec3>& colors = m_mappedColours;
    std::vector<int>& indexes = m_indexes;

    const int max = colors.size()-1;
    const int min = 0;

    glm::vec3 dest = mapRGB(target);

    // partition palette by brightness
    int upper = colors.size()-1;
    int lower = 0;
    while(lower <= upper) {
        while(colors[lower].b < lower && lower <= max) lower++;
        while(colors[upper].b > upper && upper >= min) upper--;

        if(lower < upper) {
            std::swap(colors[lower], colors[upper]);
            std::swap(indexes[lower], indexes[upper]);
        }
    }

    // select color from each partition
    int i = std::max_element(
        colors.begin(),
        colors.begin()+upper,
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - colors.begin();

    int j = std::max_element(
        colors.begin()+upper,
        colors.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) > glm::distance(dest, max);
        }) - colors.begin();

    // blend between colors
    glm::vec3 darker = colors[i];
    glm::vec3 lighter = colors[j];

    glm::vec3 dir = glm::normalize(lighter - darker);
    float t = glm::dot(dest - darker, dir) / glm::distance(lighter, darker);
    t = glm::clamp(t, 0.f, 1.f);

    return glm::mix(get(i), get(j), t);
}

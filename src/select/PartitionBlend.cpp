#include "PartitionBlend.h"
#include "glm/common.hpp"
#include "glm/geometric.hpp"

#include <algorithm>
#include <numeric>

PartitionBlend::PartitionBlend(const Palette& palette, Func fn, IColorSpace* colorSpace)
    : m_palette(palette), m_mappedColors(palette.colors()), m_pColorSpace(colorSpace), m_func(fn)

{
    if (m_pColorSpace) {
        std::transform(
            m_mappedColors.begin(),
            m_mappedColors.end(),
            m_mappedColors.begin(),
            [=](const glm::vec3& c) { return m_pColorSpace->fromRGB(c); }
        );
    }
}

glm::vec3 PartitionBlend::mapRGB(glm::vec3 col) const {
    if (!m_pColorSpace)
        return col;
    return m_pColorSpace->fromRGB(col);
}

glm::vec3 PartitionBlend::select(glm::vec3& target) {
    std::vector<int> index(m_palette.colors().size());
    std::iota(index.begin(), index.end(), 0);

    glm::vec3 dest = mapRGB(target);

    auto boundary = std::partition(index.begin(), index.end(), [dest, this](int i) {
        return m_func(dest, m_mappedColors[i]);
    });

    auto left = std::min_element(index.begin(), boundary, [dest, this](int i, int j) {
        glm::vec3 v = m_mappedColors[i];
        glm::vec3 max = m_mappedColors[j];
        return glm::distance(dest, v) < glm::distance(dest, max);
    });

    auto right = std::min_element(boundary, index.end(), [dest, this](int i, int j) {
        glm::vec3 v = m_mappedColors[i];
        glm::vec3 max = m_mappedColors[j];
        return glm::distance(dest, v) < glm::distance(dest, max);
    });

    // Bad colour
    if (left == boundary) {
        return m_palette.colors()[*right];
    }
    if (right == index.end()) {
        return m_palette.colors()[*left];
    }
    if(left == right) {
        return m_palette.colors()[*left];
    }
    int i = *left, j = *right;

    // blend between colors
    glm::vec3 darker = m_mappedColors[i];
    glm::vec3 lighter = m_mappedColors[j];

    glm::vec3 dir = glm::normalize(lighter - darker);
    float t = glm::dot(dest - darker, dir) / glm::distance(lighter, darker);
    if(t < 0) {
        return {1, 1, 0};
    }
    if(t > 1) {
        return {1, 0, 1};
    }

    return glm::mix(m_palette.colors()[i], m_palette.colors()[j], t);
}

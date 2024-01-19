#include "PartitionBlend.h"
#include "glm/common.hpp"
#include "glm/geometric.hpp"

#include <algorithm>
#include <numeric>


glm::vec3 PartitionBlend::process(glm::vec3 dest) {
    std::vector<int> index(m_points.size());
    std::iota(index.begin(), index.end(), 0);


    auto boundary = std::partition(index.begin(), index.end(), [dest, this](int i) {
        return m_partition(dest, m_points[i]);
    });

    auto left = std::min_element(index.begin(), boundary, [dest, this](int i, int j) {
        glm::vec3 v = m_points[i];
        glm::vec3 max = m_points[j];
        return glm::distance(dest, v) < glm::distance(dest, max);
    });

    auto right = std::min_element(boundary, index.end(), [dest, this](int i, int j) {
        glm::vec3 v = m_points[i];
        glm::vec3 max = m_points[j];
        return glm::distance(dest, v) < glm::distance(dest, max);
    });

    // Bad colour
    if (left == boundary) {
        return m_points[*right];
    }
    if (right == index.end()) {
        return m_points[*left];
    }
    if(left == right) {
        return m_points[*left];
    }
    int i = *left, j = *right;

    // blend between colors
    glm::vec3 darker = m_points[i];
    glm::vec3 lighter = m_points[j];

    glm::vec3 dir = glm::normalize(lighter - darker);
    float t = glm::dot(dest - darker, dir) / glm::distance(lighter, darker);
    t = glm::clamp(t, 0.f, 1.f);


    return glm::mix(m_points[i], m_points[j], t);
}

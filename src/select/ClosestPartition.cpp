#include "ClosestPartition.h"
#include "glm/geometric.hpp"
#include <algorithm>

glm::vec3 ClosestPartition::process(glm::vec3 dest) {
    auto cmp = [&](const glm::vec3& v, const glm::vec3& min) {
        return glm::distance(dest, v) < glm::distance(dest, min);
    };

    glm::vec3 closest = *std::min_element(m_points.begin(), m_points.end(), cmp);

    glm::vec3 dir = closest - dest;
    auto boundary = std::partition(m_points.begin(), m_points.end(), [=](const glm::vec3& v) {
        return glm::dot(v - dest, dir) < 0;
    });

    // Single partition
    if (boundary == m_points.end() || boundary == m_points.begin()) {
        return closest;
    }

    glm::vec3 col1 = *std::min_element(m_points.begin(), boundary, cmp);
    glm::vec3 col2 = *std::min_element(boundary, m_points.end(), cmp);

    dir = glm::normalize(col1 - col2);
    float t = glm::dot(dest - col2, dir) / glm::distance(col1, col2);
    t = glm::clamp(t, 0.f, 1.f);

    return glm::mix(col1, col2, t);
}

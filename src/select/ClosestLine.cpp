#include "ClosestLine.h"
#include "glm/geometric.hpp"
#include <algorithm>

glm::vec3 ClosestLine::process(glm::vec3 dest) {

    glm::vec3 col1 = *std::min_element(
        m_points.begin(),
        m_points.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(dest, v) < glm::distance(dest, max);
        });

    glm::vec3 dir = glm::normalize(col1-dest);

    glm::vec3 col2 = *std::min_element(
        m_points.begin(),
        m_points.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            if(v == col1) return false;
            return glm::distance(dest, v) < glm::distance(dest, max);
        });

    if(col1 == col2) return col1;

    dir = glm::normalize(col2 - col1);
    float t = glm::dot(dir, dest - col1) / glm::distance(col1, col2);
    t = glm::clamp(t, 0.f, 1.f);

    return glm::mix(col1, col2, t);
}

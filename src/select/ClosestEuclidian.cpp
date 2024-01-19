#include "ClosestEuclidian.h"
#include "glm/geometric.hpp"
#include <algorithm>

glm::vec3 ClosestEuclidian::process(glm::vec3 in) {
    return *std::min_element(
        m_points.begin(),
        m_points.end(),
        [&](const glm::vec3& v, const glm::vec3& max) {
            return glm::distance(in, v) < glm::distance(in, max);
        }
    );
}

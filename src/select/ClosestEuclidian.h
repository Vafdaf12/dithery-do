#pragma once

#include "pipeline/PipelineStep.h"

#include <vector>

class ClosestEuclidian : public PipelineStep {
public:
    ClosestEuclidian(const std::vector<glm::vec3>& points) : m_points(points) {}

private:
    glm::vec3 process(glm::vec3 in) override;

    std::vector<glm::vec3> m_points;
};

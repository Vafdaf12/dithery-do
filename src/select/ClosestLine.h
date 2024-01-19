#pragma once

#include "pipeline/PipelineStep.h"

#include <vector>

class ClosestLine : public PipelineStep {
public:
    ClosestLine(const std::vector<glm::vec3>& points) : m_points(points) {}

private:
    glm::vec3 process(glm::vec3 point) override;

    std::vector<glm::vec3> m_points;
};

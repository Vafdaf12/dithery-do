#pragma once

#include "pipeline/PipelineStep.h"

#include <vector>

class PartitionBlend : public PipelineStep {
public:
    PartitionBlend(const std::vector<glm::vec3>& points, glm::vec3 partitionVec)
        : m_points(points), m_partitionVector(partitionVec) {}

private:
    glm::vec3 process(glm::vec3 target) override;

    glm::vec3 m_partitionVector;
    std::vector<glm::vec3> m_points;
};

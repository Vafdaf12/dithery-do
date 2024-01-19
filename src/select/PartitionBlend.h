#pragma once

#include "pipeline/PipelineStep.h"

#include <functional>

class PartitionBlend : public PipelineStep{
public:
    using Func = std::function<bool(glm::vec3 target, glm::vec3 value)>;

    PartitionBlend(const std::vector<glm::vec3>& points, Func fn) : m_points(points), m_partition(fn){}
private:

    glm::vec3 process(glm::vec3 target) override;

    Func m_partition;
    std::vector<glm::vec3> m_points;
};

#pragma once

#include "glm/ext/vector_float3.hpp"
#include <memory>

class PipelineStep {
public:
    virtual ~PipelineStep() = default;

    // Exeuctes the pipeline, returning the resulting color
    glm::vec3 execute(glm::vec3 in);

    // Appends a step to the end of the pipeline
    void append(std::unique_ptr<PipelineStep> step);

protected:
    virtual glm::vec3 process(glm::vec3 in) = 0;

private:
    std::unique_ptr<PipelineStep> m_next = nullptr;
};

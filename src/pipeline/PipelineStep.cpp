#include "PipelineStep.h"

void PipelineStep::append(std::unique_ptr<PipelineStep> step) {
    if (m_next) {
        m_next->append(std::move(step));
    } else {
        m_next = std::move(step);
    }
}

glm::vec3 PipelineStep::execute(glm::vec3 step) {
    step = process(step);
    if(m_next) {
        return m_next->execute(step);
    }
    else {
        return step;
    }
}

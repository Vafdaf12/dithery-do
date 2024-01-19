#include "PipelineStep.h"

#include <iostream>
void PipelineStep::append(std::unique_ptr<PipelineStep> step) {
    if (m_next) {
        std::cout << "Next" << std::endl;
        m_next->append(std::move(step));
    } else {
        std::cout << "---- APPEND" << std::endl;
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

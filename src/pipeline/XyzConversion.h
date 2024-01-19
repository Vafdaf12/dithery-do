#pragma once

#include "pipeline/PipelineStep.h"

class XyzConversion : public PipelineStep {
public:
    XyzConversion(bool toColor = false) : m_toColor(toColor) {}

private:
    glm::vec3 process(glm::vec3 in) override;
    glm::vec3 fromRGB(glm::vec3 color) const;
    glm::vec3 toRGB(glm::vec3 color) const;

    bool m_toColor;
};

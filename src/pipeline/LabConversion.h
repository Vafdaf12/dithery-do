#pragma once

#include "pipeline/PipelineStep.h"

class LabConversion : public PipelineStep {
public:
    LabConversion(glm::vec3 illuminant, bool toColor = false)
        : m_toColor(toColor), m_illuminant(illuminant) {}

    static const glm::vec3 D65;
    static const glm::vec3 D50;

private:
    static float func(float t);
    static float funcInv(float t);

    static constexpr double DELTA = 6/29.f;

    glm::vec3 process(glm::vec3 in) override;
    glm::vec3 fromXyz(glm::vec3 color) const;
    glm::vec3 toXyz(glm::vec3 color) const;

    bool m_toColor;
    glm::vec3 m_illuminant;
};

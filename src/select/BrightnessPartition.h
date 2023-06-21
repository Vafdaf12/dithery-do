#pragma once

#include "IColorSelector.h"
#include "../Palette.h"
#include "../color/LabColorSpace.h"

class BrightnessPartition : public IColorSelector {
public:
    BrightnessPartition(const Palette& palette);
    glm::vec3 select(glm::vec3& target) override;

private:
    glm::vec3 mapRGB(glm::vec3 col) const;
    glm::vec3 get(int i) const;


    const Palette& m_palette;
    LabColorSpace m_colorSpace;

    std::vector<glm::vec3> m_mappedColours;
    std::vector<int> m_indexes;
};

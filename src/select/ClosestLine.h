#pragma once

#include "IColorSelector.h"
#include "../Palette.h"
#include "../color/IColorSpace.h"

class ClosestLine : public IColorSelector {
public:
    ClosestLine(const Palette& palette, IColorSpace* colorSpace = nullptr);
    glm::vec3 select(glm::vec3& target) override;

private:
    glm::vec3 mapRGB(glm::vec3 col) const;

    const Palette& m_palette;
    IColorSpace* m_pColorSpace;

    std::vector<glm::vec3> m_mappedColours;
};

#pragma once

#include "IColorSelector.h"
#include "../Palette.h"

#include "../color/IColorSpace.h"

class ClosestEuclidian : public IColorSelector {
public:
    ClosestEuclidian(const Palette& palette, IColorSpace* colorSpace = nullptr);
    glm::vec3 select(glm::vec3& target) override;

private:
    const Palette& m_palette;
    IColorSpace* m_pColorSpace;

    std::vector<glm::vec3> m_mappedColours;
};

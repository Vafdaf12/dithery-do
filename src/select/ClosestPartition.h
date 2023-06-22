#pragma once

#include "IColorSelector.h"
#include "../Palette.h"
#include "../color/IColorSpace.h"

class ClosestPartition : public IColorSelector {
public:
    ClosestPartition(const Palette& palette, IColorSpace* colorSpace = nullptr);

    glm::vec3 select(glm::vec3& target) override;

private:
    glm::vec3 mapRGB(glm::vec3 col) const;
    glm::vec3 get(int i) const;

    const Palette& m_palette;
    IColorSpace* m_pColorSpace;

    std::vector<glm::vec3> m_mappedColours;
    std::vector<int> m_indexes;
};

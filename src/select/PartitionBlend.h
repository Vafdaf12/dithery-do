#pragma once

#include "IColorSelector.h"
#include "Palette.h"
#include "color/IColorSpace.h"

#include <functional>

class PartitionBlend : public IColorSelector{
public:
    using Func = std::function<bool(glm::vec3 target, glm::vec3 value)>;

    PartitionBlend(const Palette& palette, Func fn, IColorSpace* colorSpace = nullptr);
    glm::vec3 select(glm::vec3& target) override;
private:
    glm::vec3 mapRGB(glm::vec3 col) const;
    glm::vec3 get(int i) const;

    const Palette& m_palette;
    IColorSpace* m_pColorSpace;
    Func m_func;

    std::vector<glm::vec3> m_mappedColors;
};

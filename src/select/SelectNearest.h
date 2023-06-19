#pragma once

#include "IColorSelector.h"
#include "../Palette.h"

class SelectNearest : public IColorSelector {
public:
    SelectNearest(const Palette& palette, bool xyz = false);
    glm::vec3 select(glm::vec3& target) override;

private:
    static glm::vec3 toXYZ(glm::vec3 rgb);

    const Palette& m_palette;
    bool m_xyz;
};

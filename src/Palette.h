#pragma once

#include "glm/ext/vector_float3.hpp"

#include <ostream>
#include <string>
#include <vector>

class Palette {
public:
    bool loadFromFile(const std::string& filename);

    inline const std::vector<glm::vec3>& colors() const { return m_colors; }

    friend std::ostream& operator<<(std::ostream& out, const Palette& p);
private:
    std::vector<glm::vec3> m_colors;
};

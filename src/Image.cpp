#include "Image.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <unordered_map>

bool Image::loadFromFile(const std::string& filename) {
    m_pData = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, 3);
    std::cout << "Error: " << stbi_failure_reason() << std::endl;
    return m_pData != nullptr;
}
bool Image::writeToFile(const std::string& filename) const {
    int res = stbi_write_jpg(filename.c_str(), m_width, m_height, m_channels, m_pData, 100);
    return res == 0;
}

glm::vec3 Image::get(int x, int y) const {
    uint8_t* pos = index(x, y);
    return {
        pos[0] / 255.0f,
        pos[1] / 255.0f,
        pos[2] / 255.0f
    };
}

uint8_t* Image::index(int x, int y) const {
    assert(m_pData != nullptr);
    assert(x >= 0 && x < m_width);
    assert(y >= 0 && y < m_height);

    return m_pData + (x + y * m_width) * m_channels;
}

void Image::set(int x, int y, const glm::vec3& col) {
    uint8_t* pos = index(x, y);
    pos[0] = std::round(col.r * 255);
    pos[1] = std::round(col.g * 255);
    pos[2] = std::round(col.b * 255);
}

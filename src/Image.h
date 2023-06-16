#pragma once

#include <cstdint>
#include <string>

#include "glm/ext/vector_float3.hpp"

class Image {
public:
    Image() = default;

    // File-loading
    bool loadFromFile(const std::string& filename);
    bool writeToFile(const std::string& filename) const;

    // Pixel manipulation
    glm::vec3 get(int x, int y) const;
    void set(int x, int y, const glm::vec3& col);

    // Getters
    inline int width() const { return m_width; }
    inline int height() const { return m_height; }
    inline int numChannels() const { return m_channels; }
private:
    uint8_t* index(int x, int y) const;

    int m_width = -1;
    int m_height = -1;
    int m_channels = -1;

    uint8_t* m_pData = nullptr;
};

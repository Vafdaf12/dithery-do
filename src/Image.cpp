#include "Image.h"

#include "glm/common.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <iostream>
#include <unordered_map>

Image::Image(Image&& rhs) {
    m_pData = rhs.m_pData;
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_channels = rhs.m_channels;
}
Image::Image(const Image& rhs) {
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_channels = rhs.m_channels;

    if(!rhs.m_pData) {
        m_pData = nullptr;
        return;
    }

    m_pData = new uint8_t[m_width * m_height * m_channels];
    std::memcpy(m_pData, rhs.m_pData, m_width * m_height * m_channels);
}
Image& Image::operator=(Image&& rhs) {
    std::swap(rhs.m_pData, m_pData);
    std::swap(rhs.m_width, m_width);
    std::swap(rhs.m_height, m_height);
    std::swap(rhs.m_channels, m_channels);

    return *this;
}
Image& Image::operator=(Image& rhs) {
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_channels = rhs.m_channels;

    if(!rhs.m_pData) {
        m_pData = nullptr;
        return *this;
    }

    m_pData = new uint8_t[m_width * m_height * m_channels];
    std::memcpy(m_pData, rhs.m_pData, m_width * m_height * m_channels);

    return *this;
}
Image::~Image() {
    if(m_pData) stbi_image_free(m_pData);
}

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
    return glm::vec3(pos[0], pos[1], pos[2]) / 255.0f;
}

uint8_t* Image::index(int x, int y) const {
    assert(m_pData != nullptr);
    assert(x >= 0 && x < m_width);
    assert(y >= 0 && y < m_height);

    return m_pData + (x + y * m_width) * m_channels;
}

int toRGB(float val) {
    if(val <= 0) return 0;
    if(val >= 1) return 255;
    return std::round(val * 255);
}

void Image::set(int x, int y, const glm::vec3& col) {
    uint8_t* pos = index(x, y);
    glm::vec3 c = glm::clamp(col, 0.0f, 1.0f) * 255.0f;
    
    pos[0] = c.r;
    pos[1] = c.g;
    pos[2] = c.b;
}


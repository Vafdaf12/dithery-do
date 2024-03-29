#include "Palette.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>

std::ostream& operator<<(std::ostream& out, const Palette& p) {
    out << "Palette (" << p.m_colors.size() << ")\n";
    for (auto col : p.m_colors) {
        out << " - RGB(" << std::setprecision(2);
        out << col.r << ", " << col.g << ", " << col.b << ")\n";
    }
    return out;
}

uint32_t compressRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (r << 16) + (g << 8) + b;
}
glm::vec3 getVector(uint32_t rgb) {
    uint8_t r = (rgb >> 16) & 255;
    uint8_t g = (rgb >> 8) & 255;
    uint8_t b = rgb & 255;

    return glm::vec3(r, g, b) / 255.0f;
}

bool Palette::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
        return false;
    }

    std::set<uint32_t> set;
    std::string line;
    while (!file.eof()) {
        std::getline(file, line);
        std::size_t i = line.find('#');

        if (i == std::string::npos) {
            std::cerr << "Invalid colour \"" << line << "\" (are you missing a #?). Skipping"
                      << std::endl;
            continue;
        }
        line = line.substr(i + 1);

        std::stringstream s;
        s << std::hex << line;

        uint32_t value = 9;
        s >> value;

        int b = value & 0xFF;
        int g = (value >> 8) & 0xFF;
        int r = (value >> 16) & 0xFF;
        set.insert(compressRGB(r, g, b));
    }
    file.close();

    // copy + map distinct elements to array
    m_colors.clear();
    m_colors.reserve(set.size());
    std::transform(set.begin(), set.end(), std::back_inserter(m_colors), getVector);

    return true;
}

#include "Palette.h"

#include <fstream>
#include <iomanip>
#include <string>

std::ostream& operator<<(std::ostream& out, const Palette& p) {
    out << "Palette (" << p.m_colors.size() << ")\n";
    for(auto col : p.m_colors) {
        out << " - RGB(" << std::setprecision(2);
        out << col.r << ", " << col.g << ", " << col.b << ")\n";
    }
    return out;
}

bool Palette::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if(!file) return false;

    m_colors.clear();
    std::string line;
    while(!file.eof()) {
        std::getline(file, line);
        std::size_t i = line.find('#');

        if(i == std::string::npos) continue;
        line = line.substr(i+1);

        int r, g, b;
        sscanf(line.data(), "%02x%02x%02x", &r, &g, &b);
        m_colors.push_back(glm::vec3(r, g, b) / 255.0f);
    }
    file.close();
    return true;
}

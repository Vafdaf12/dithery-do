#include "color/LabColorSpace.h"
#include "color/XyzColorSpace.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "CLI/Validators.hpp"

#include "glm/geometric.hpp"

#include "select/IColorSelector.h"
#include "select/ClosestEuclidian.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <array>

#include "Image.h"
#include "Palette.h"

using PaletteFunc = std::function<void(Image& img, const Palette& p, int x, int y)>;

glm::vec3 convertToXYZ(glm::vec3 color) {
    color.r = ((color.r > 0.04045) ? pow((color.r + 0.055) / 1.055, 2.4) : (color.r / 12.92)) * 100.0;
    color.g = ((color.g > 0.04045) ? pow((color.g + 0.055) / 1.055, 2.4) : (color.g / 12.92)) * 100.0;
    color.b = ((color.b > 0.04045) ? pow((color.b + 0.055) / 1.055, 2.4) : (color.b / 12.92)) * 100.0;

    color.r = color.r*0.4124564 + color.g*0.3575761 + color.b*0.1804375;
    color.g = color.r*0.2126729 + color.g*0.7151522 + color.b*0.0721750;
    color.b = color.r*0.0193339 + color.g*0.1191920 + color.b*0.9503041;
    return color;
}


glm::vec3 findClosest(const glm::vec3& target, const std::vector<glm::vec3>& colors) {
    glm::vec3 xyz = convertToXYZ(target);
    return *std::min_element(colors.begin(), colors.end(),
        [&](const glm::vec3& v1, const glm::vec3& v2) {
            return glm::distance(convertToXYZ(v1), xyz) < glm::distance(convertToXYZ(v2), xyz);
        });
}

glm::vec3 snapColor(const glm::vec3& col, const Palette& p) {
    const std::vector<glm::vec3>& colors = p.colors();

    glm::vec3 col1 = findClosest(col, colors);

    glm::vec3 delta = convertToXYZ(col1) - convertToXYZ(col);

    std::vector<glm::vec3> closest;
    std::copy_if(
        colors.begin(),
        colors.end(),
        std::back_inserter(closest),
        [&](const glm::vec3& v) {
            glm::vec3 d = convertToXYZ(v) - convertToXYZ(col1);
            return glm::dot(d, delta) < 0;
        }
    );

    if(closest.empty()) return col1;
    glm::vec3 col2 = findClosest(col, closest);


    glm::vec3 dirRoot = glm::normalize(col2 - col1);
    glm::vec3 dirCol = glm::normalize(col - col1);
    float t = glm::clamp(glm::dot(dirRoot, dirCol), 0.f, 1.f);
    return glm::mix(col1, col2, t);
}

void diffuse_steinberg(Image& img, const glm::vec3& err, int x, int y) {
    glm::vec3 pix, i;

    i = img.get(x+1, y);
    pix = i + err * (7 / 16.f);
    img.set(x+1, y, pix);

    i = img.get(x-1, y+1);
    pix = i + err * (3 / 16.f);
    img.set(x-1, y+1, pix);

    i = img.get(x, y+1);
    pix = i + err * (5 / 16.f);
    img.set(x, y+1, pix);

    i = img.get(x+1, y+1);
    pix = i + err * (1 / 16.f);
    img.set(x+1, y+1, pix);
}

enum ColorSpace {
    CS_RGB = 0,
    CS_XYZ = 1,
    CS_LAB = 2
};


int main(int argc, char** argv) {
    CLI::App app{"This is a CLI utility"};

    std::string inFile = "";
    std::string outFile = "output.jpg";
    std::string paletteFile = "palette.txt";
    ColorSpace colorSpace;

    app.add_option("-f, --file", inFile, "The input image")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("-p, --palette", paletteFile, "Path to palette file")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("--color-space", colorSpace, "The color space to use for selection");

    app.add_option("-o, --output", outFile, "The output image");


    CLI11_PARSE(app, argc, argv);

    int w, h, n;


    std::cout << "Read File: " << inFile << std::endl;
    std::cout << "Write File: " << outFile << std::endl;
    std::cout << "Palette: " << paletteFile << std::endl;

    Image image;
    Palette palette;

    image.loadFromFile(inFile);
    palette.loadFromFile(paletteFile);

    IColorSpace* space;
    switch(colorSpace) {
        case CS_XYZ:
            space = new XyzColorSpace;
            break;
        case CS_LAB:
            space = new LabColorSpace(LabColorSpace::D50);
            break;
        default:
            space = nullptr;
            break;
    }

    IColorSelector* selector = new ClosestEuclidian(palette, space);
    for(int y = 0; y < image.height(); y++) {
        for(int x = 1; x < image.width(); x++) {
            glm::vec3 src = image.get(x, y);
            glm::vec3 dest = selector->select(src);
            image.set(x, y, dest);
        }
    }

    image.writeToFile(outFile);
}

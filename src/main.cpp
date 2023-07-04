#include "color/LabColorSpace.h"
#include "color/XyzColorSpace.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "CLI/Validators.hpp"

#include "glm/geometric.hpp"

#include "select/ClosestPartition.h"
#include "select/ClosestLine.h"
#include "select/BrightnessPartition.h"
#include "select/IColorSelector.h"
#include "select/ClosestEuclidian.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <string>
#include <iostream>

#include "Image.h"
#include "Palette.h"
#include "vulkan/Vulkan.h"

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

enum class ColorSpace {
    Rgb,
    Xyz,
    Lab
};

enum class SelectionAlgo {
    ClosestEuclidian = 0,
    ClosestMix,
    ClosestPartition,
    BrightnessPartition
};


std::ostream& operator<<(std::ostream& out, const ColorSpace& space) {
    switch(space) {
        case ColorSpace::Rgb:
            return (out << "RGB");
        case ColorSpace::Xyz:
            return (out << "XYZ");
        case ColorSpace::Lab:
            return (out << "L*a*b (D50)");
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const SelectionAlgo& space) {
    switch(space) {
        case SelectionAlgo::ClosestEuclidian:
            return (out << "Closest Euclidian");
        case SelectionAlgo::ClosestMix:
            return (out << "Closest Mix");
        case SelectionAlgo::ClosestPartition:
            return (out << "Closest Partition");
        case SelectionAlgo::BrightnessPartition:
            return (out << "Closest Euclidian");
    }
    return out;
}


int main(int argc, char** argv) {
    Vulkan app;
    app.run();
    /*
    CLI::App app{"A tool to \"paint\" images just like an artist would "};

    std::string inFile;
    std::string outFile;
    std::string paletteFile;

    ColorSpace colorSpace = ColorSpace::Rgb;
    SelectionAlgo selectionAlgorithm = SelectionAlgo::ClosestEuclidian;

    app.add_option("-f, --file", inFile, "The input image")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("-p, --palette", paletteFile, "Path to palette file")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("-o, --output", outFile, "The output image")
        ->required(true);

    app.add_option("--space", colorSpace, "The color space to use (Default: RGB)");
    app.add_option("--select", selectionAlgorithm, "The selection algorithm to use (Default: Closest Euclidian)");

    CLI11_PARSE(app, argc, argv);

    int w, h, n;

    std::cout << inFile << " -> " << outFile << std::endl;
    std::cout << "Colour Space: " << colorSpace << std::endl;
    std::cout << "Selection Algorithm: " << selectionAlgorithm << std::endl;

    Image image;
    Palette palette;

    image.loadFromFile(inFile);
    palette.loadFromFile(paletteFile);

    std::cout << palette << std::endl;

    IColorSpace* space;
    switch(colorSpace) {
        case ColorSpace::Rgb:
            space = nullptr;
            break;
        case ColorSpace::Xyz:
            space = new XyzColorSpace;
            break;
        case ColorSpace::Lab:
            space = new LabColorSpace(LabColorSpace::D50);
            break;
    }

    IColorSelector* selector = nullptr;
    switch(selectionAlgorithm) {
        case SelectionAlgo::ClosestEuclidian:
            selector = new ClosestEuclidian(palette, space);
            break;
        case SelectionAlgo::ClosestMix:
            selector = new ClosestLine(palette, space);
            break;
        case SelectionAlgo::ClosestPartition:
            selector = new ClosestPartition(palette, space);
            break;
        case SelectionAlgo::BrightnessPartition:
            selector = new BrightnessPartition(palette);
            break;
    }
    std::cout << std::endl;


    for(int y = 0; y < image.height(); y++) {
        if(y % (image.height() / 10) == 0) {
            float progress = float(y) / image.height() * 100.f;
            std::cout << "Progress: " << progress << "%\n";
        }
        for(int x = 0; x < image.width(); x++) {
            glm::vec3 src = image.get(x, y);
            glm::vec3 dest = selector->select(src);
            image.set(x, y, dest);
        }
    }
    std::cout << std::endl;

    image.writeToFile(outFile);
    */
}

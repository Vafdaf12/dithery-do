#include "color/LabColorSpace.h"
#include "color/XyzColorSpace.h"

#include "select/BrightnessPartition.h"
#include "select/ClosestEuclidian.h"
#include "select/ClosestLine.h"
#include "select/ClosestPartition.h"
#include "select/IColorSelector.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "argparse/argparse.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Image.h"
#include "Palette.h"

void diffuse_steinberg(Image& img, const glm::vec3& err, int x, int y) {
    glm::vec3 pix, i;

    i = img.get(x + 1, y);
    pix = i + err * (7 / 16.f);
    img.set(x + 1, y, pix);

    i = img.get(x - 1, y + 1);
    pix = i + err * (3 / 16.f);
    img.set(x - 1, y + 1, pix);

    i = img.get(x, y + 1);
    pix = i + err * (5 / 16.f);
    img.set(x, y + 1, pix);

    i = img.get(x + 1, y + 1);
    pix = i + err * (1 / 16.f);
    img.set(x + 1, y + 1, pix);
}

enum class ColorSpace { Rgb = 0, Xyz, Lab };

enum class Algorithm {
    ClosestEuclid = 0,
    ClosestLine,
    ClosestPartition,
    ClosestTri,
};
ColorSpace space_from_string(const std::string& val) {
    if (val == "rgb") {
        return ColorSpace::Rgb;
    } else if (val == "xyz") {
        return ColorSpace::Xyz;
    } else if (val == "lab") {
        return ColorSpace::Lab;
    } else {
        throw std::invalid_argument("Unsupported color space: " + val);
    }
}
Algorithm algo_from_string(const std::string& val) {
    if (val == "euclid") {
        return Algorithm::ClosestEuclid;
    } else if (val == "line") {
        return Algorithm::ClosestLine;
    } else if (val == "partition") {
        return Algorithm::ClosestPartition;
    } else if (val == "tri") {
        return Algorithm::ClosestTri;
    } else {
        throw std::invalid_argument("Unsupported algorithm: " + val);
    }
}


int main(int argc, char** argv) {
    argparse::ArgumentParser cli("Dithery Do");

    cli.add_argument("input").help("The input image");
    cli.add_argument("palette").help("Path to palette file");

    cli.add_argument("-o").help("Path of output image").default_value("output.jpg");

    cli.add_argument("-a", "-algo")
        .help("The color selection algorithm to use")
        .choices("euclid", "line", "partition", "tri")
        .required();

    cli.add_argument("-s", "-space")
        .help("The color space to use for selection")
        .choices("rgb", "xyz", "lab")
        .default_value("rgb")
        .required();

    // Parse the output
    std::string inputPath;
    std::string outputPath;
    std::string palettePath;

    Algorithm algorithm;
    ColorSpace space;

    try {
        cli.parse_args(argc, argv);

        algorithm = algo_from_string(cli.get("-algo"));
        space = space_from_string(cli.get("-space"));
        inputPath = cli.get("input");
        outputPath = cli.get("-o");
        palettePath = cli.get("palette");

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << cli;
        return 1;
    }

    std::cout << "Read File: " << inputPath << std::endl;
    std::cout << "Write File: " << outputPath << std::endl;
    std::cout << "Palette: " << palettePath << std::endl;

    Image image;
    Palette palette;

    image.loadFromFile(inputPath);
    palette.loadFromFile(palettePath);

    IColorSpace* spaceMapping;
    switch (space) {
    case ColorSpace::Rgb: spaceMapping = nullptr; break;
    case ColorSpace::Xyz: spaceMapping = new XyzColorSpace; break;
    case ColorSpace::Lab: spaceMapping = new LabColorSpace(LabColorSpace::D50); break;
    }

    IColorSelector* selector = new ClosestEuclidian(palette, spaceMapping);
    switch (algorithm) {

    case Algorithm::ClosestEuclid: selector = new ClosestEuclidian(palette, spaceMapping); break;
    case Algorithm::ClosestLine: selector = new ClosestLine(palette, spaceMapping); break;
    case Algorithm::ClosestPartition: selector = new ClosestPartition(palette, spaceMapping); break;
    case Algorithm::ClosestTri: selector = new BrightnessPartition(palette); break;
    }

    for (int y = 0; y < image.height(); y++) {
        for (int x = 1; x < image.width(); x++) {
            glm::vec3 src = image.get(x, y);
            glm::vec3 dest = selector->select(src);
            image.set(x, y, dest);
        }
    }

    image.writeToFile(outputPath);

}

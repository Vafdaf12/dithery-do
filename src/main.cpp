#include "color/LabColorSpace.h"
#include "color/XyzColorSpace.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "CLI/Validators.hpp"

#include "glm/geometric.hpp"

#include "select/ClosestMix.h"
#include "select/IColorSelector.h"
#include "select/ClosestEuclidian.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <string>
#include <iostream>

#include "Image.h"
#include "Palette.h"

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
    CS_XYZ,
    CS_LAB
};

enum SelectionAlgo {
    SA_CLOSEST_EUCLID = 0,
    SA_CLOSEST_MIX,
};


int main(int argc, char** argv) {
    CLI::App app{"This is a CLI utility"};

    std::string inFile = "";
    std::string outFile = "output.jpg";
    std::string paletteFile = "palette.txt";
    ColorSpace colorSpace = CS_RGB;
    SelectionAlgo selectionAlgorithm = SA_CLOSEST_EUCLID;

    app.add_option("-f, --file", inFile, "The input image")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("-p, --palette", paletteFile, "Path to palette file")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("--color-space", colorSpace, "The color space to use for selection");
    app.add_option("--select", selectionAlgorithm, "The color selection algorithm to use");

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
        case CS_RGB:
            space = nullptr;
            break;
        case CS_XYZ:
            space = new XyzColorSpace;
            break;
        case CS_LAB:
            space = new LabColorSpace(LabColorSpace::D50);
            break;
    }

    IColorSelector* selector = new ClosestMix(palette, space);
    switch(selectionAlgorithm) {
        case SA_CLOSEST_EUCLID:
            selector = new ClosestEuclidian(palette, space);
            break;
        case SA_CLOSEST_MIX:
            selector = new ClosestMix(palette, space);
            break;
    }

    for(int y = 0; y < image.height(); y++) {
        for(int x = 1; x < image.width(); x++) {
            glm::vec3 src = image.get(x, y);
            glm::vec3 dest = selector->select(src);
            image.set(x, y, dest);
        }
    }

    image.writeToFile(outFile);
}

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
    SA_CLOSEST_LINE,
    SA_CLOSEST_PARTITION,
    SA_CLOSEST_TRI,
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


    std::cout << inFile << " -> " << outFile << std::endl;

    Image image;
    Palette palette;

    image.loadFromFile(inFile);
    palette.loadFromFile(paletteFile);

    std::cout << palette << std::endl;

    IColorSpace* space;
    std::cout << "Colour Space: ";
    switch(colorSpace) {
        case CS_RGB:
            std::cout << "RGB";
            space = nullptr;
            break;
        case CS_XYZ:
            std::cout << "XYZ";
            space = new XyzColorSpace;
            break;
        case CS_LAB:
            std::cout << "L*a*b (D50)";
            space = new LabColorSpace(LabColorSpace::D50);
            break;
    }
    std::cout << std::endl;

    std::cout << "Algorithm: ";

    IColorSelector* selector = nullptr;
    switch(selectionAlgorithm) {
        case SA_CLOSEST_EUCLID:
            std::cout << "Closest Euclidian";
            selector = new ClosestEuclidian(palette, space);
            break;
        case SA_CLOSEST_LINE:
            std::cout << "Closest Mix" << std::endl;
            selector = new ClosestLine(palette, space);
            break;
        case SA_CLOSEST_PARTITION:
            std::cout << "Closest Partition" << std::endl;
            selector = new ClosestPartition(palette, space);
            break;
        case SA_CLOSEST_TRI:
            std::cout << "Brightness Partition" << std::endl;
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
}

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "CLI/Validators.hpp"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>

#include "Image.h"
#include "Palette.h"

using PaletteFunc = std::function<void(Image& img, const Palette& p, int x, int y)>;
glm::vec3 snapColor(const glm::vec3& col, const Palette& p) {
    const std::vector<glm::vec3>& colors = p.colors();

    const glm::vec3* col2 = nullptr;
    const glm::vec3* col1 = &colors[0];

    float diff = glm::length(*col1 - col);
    for(int i = 1; i < colors.size(); i++) {
        float dist = glm::length(colors[i] - col);
        if(dist >= diff) continue;
        diff = dist;
        col2 = col1;
        col1 = &colors[i];
    }
    if(!col2) return *col1;

    glm::vec3 dirRoot = glm::normalize(*col2 - *col1);
    glm::vec3 dirCol = glm::normalize(col - *col1);
    float t = glm::clamp(glm::dot(dirRoot, dirCol), 0.f, 1.f);

    return *col1 + (*col2 - *col1) * t;
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


int main(int argc, char** argv) {
    CLI::App app{"This is a CLI utility"};

    std::string inFile = "";
    std::string outFile = "output.jpg";
    std::string paletteFile = "palette.txt";

    app.add_option("-f, --file", inFile, "The input image")
        ->required(true)
        ->check(CLI::ExistingFile);

    app.add_option("-p, --palette", paletteFile, "Path to palette file")
        ->required(true)
        ->check(CLI::ExistingFile);

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


    for(int y = 0; y < image.height()-1; y++) {
        for(int x = 1; x < image.width()-1; x++) {
            glm::vec3 src = image.get(x, y);
            glm::vec3 dest = snapColor(src, palette);
            image.set(x, y, dest);
            diffuse_steinberg(image, src - dest, x, y);
        }
    }

    image.writeToFile(outFile);
}

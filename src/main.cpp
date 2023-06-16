#include "glm/ext/vector_float3.hpp"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "CLI/Validators.hpp"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <cstdint>
#include <string>

#include "Image.h"

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
    if(!image.loadFromFile(inFile)) {
        std::cout << "Could not load image" << std::endl;
        return 1;
    }

    image.set(1, 0, {1, 0, 0});
    image.writeToFile(outFile);

    glm::vec3 vec{1, 2, 3};
    std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
}

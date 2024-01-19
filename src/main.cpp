#include "Palette.h"
#include "color/LabColorSpace.h"
#include "color/XyzColorSpace.h"
#include "glm/common.hpp"
#include "pipeline/LabConversion.h"
#include "pipeline/PipelineStep.h"
#include "pipeline/XyzConversion.h"
#include "select/ClosestEuclidian.h"
#include "select/ClosestLine.h"

#include "argparse/argparse.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

enum class ColorSpace : int { Rgb = 0, Xyz, Lab50, Lab65 };

enum class Algorithm : int {
    ClosestEuclid = 0,
    ClosestLine,
    Blend,
};
ColorSpace space_from_string(const std::string& val) {
    if (val == "rgb") {
        return ColorSpace::Rgb;
    } else if (val == "xyz") {
        return ColorSpace::Xyz;
    } else if (val == "lab50") {
        return ColorSpace::Lab50;
    } else if (val == "lab65") {
        return ColorSpace::Lab65;
    } else {
        throw std::invalid_argument("Unsupported color space: " + val);
    }
}
Algorithm algo_from_string(const std::string& val) {
    if (val == "euclid") {
        return Algorithm::ClosestEuclid;
    } else if (val == "line") {
        return Algorithm::ClosestLine;
    } else if (val == "blend_chan") {
        return Algorithm::Blend;
    } else {
        throw std::invalid_argument("Unsupported algorithm: " + val);
    }
}
const std::string& validate_path(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument("Path not found: " + path);
    }
    return path;
}
uint32_t color_key(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (r << 16) + (g << 8) + b;
}
glm::vec3 color_vec(uint32_t key) {
    uint8_t r = (key >> 16) & 255;
    uint8_t g = (key >> 8) & 255;
    uint8_t b = key & 255;

    return glm::vec3(r, g, b) / 255.0f;
}

bool brightness(int i, glm::vec3 target, glm::vec3 value) { return value[i] < target[i]; }
int main(int argc, char** argv) {
    {
        glm::vec3 color = {1, 1, 1};
        LabColorSpace space(LabColorSpace::D65);

        std::unique_ptr<PipelineStep> pipeline = std::make_unique<XyzConversion>();
        pipeline->append(std::make_unique<LabConversion>(LabColorSpace::D65));

        glm::vec3 piped = pipeline->execute(color);
        glm::vec3 spaced = space.fromRGB(color);
        std::cout << piped.x << " " << piped.y << " " << piped.z << std::endl;
        std::cout << spaced.x << " " << spaced.y << " " << spaced.z << std::endl;

        pipeline->append(std::make_unique<LabConversion>(LabColorSpace::D65, true));
        pipeline->append(std::make_unique<XyzConversion>(true));

        piped = pipeline->execute(color);
        spaced = space.toRGB(spaced);

        std::cout << piped.x << " " << piped.y << " " << piped.z << std::endl;
        std::cout << spaced.x << " " << spaced.y << " " << spaced.z << std::endl;
        std::cout << "----------------------" << std::endl;
    }
    {
        glm::vec3 color = {1, 1, 1};
        XyzColorSpace space;

        std::unique_ptr<PipelineStep> pipeline = std::make_unique<XyzConversion>();

        glm::vec3 piped = pipeline->execute(color);
        glm::vec3 spaced = space.fromRGB(color);
        std::cout << piped.x << " " << piped.y << " " << piped.z << std::endl;
        std::cout << spaced.x << " " << spaced.y << " " << spaced.z << std::endl;

        pipeline->append(std::make_unique<XyzConversion>(true));

        piped = pipeline->execute(color);
        spaced = space.toRGB(spaced);

        std::cout << piped.x << " " << piped.y << " " << piped.z << std::endl;
        std::cout << spaced.x << " " << spaced.y << " " << spaced.z << std::endl;
        std::cout << "----------------------" << std::endl;
    }
    argparse::ArgumentParser cli("Dithery Do");

    cli.add_argument("input").help("The input image");
    cli.add_argument("palette").help("Path to palette file");

    cli.add_argument("-o").help("Path of output image").default_value("output.jpg");

    cli.add_argument("-a", "-algo")
        .help("The color selection algorithm to use")
        .choices("euclid", "line", "blend_chan")
        .required();

    cli.add_argument("-s", "-space")
        .help("The color space to use for selection")
        .choices("rgb", "xyz", "lab50", "lab65")
        .default_value("rgb")
        .required();

    cli.add_argument("-c", "--channel")
        .help("The channel to use for partition blend")
        .default_value(0)
        .choices(0, 1, 2)
        .scan<'i', int>()
        .required();

    // Parse the output
    std::string inputPath;
    std::string outputPath;
    std::string palettePath;

    Algorithm algorithm;
    ColorSpace space;
    int channel;

    try {
        cli.parse_args(argc, argv);

        algorithm = algo_from_string(cli.get("-algo"));
        space = space_from_string(cli.get("-space"));
        inputPath = validate_path(cli.get("input"));
        palettePath = validate_path(cli.get("palette"));
        outputPath = cli.get("-o");
        channel = cli.get<int>("-c");

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << cli;
        return 1;
    }

    std::cout << "Read File: " << inputPath << std::endl;
    std::cout << "Write File: " << outputPath << std::endl;
    std::cout << "Palette: " << palettePath << std::endl;
    std::cout << "Algorithm: " << int(algorithm) << std::endl;
    std::cout << "Color Space: " << int(space) << std::endl;

    // Load input data
    int width, height, channels;
    uint8_t* data = stbi_load(inputPath.c_str(), &width, &height, &channels, 3);
    channels = 3;
    if (!data) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return 1;
    }
    Palette palette;
    if (!palette.loadFromFile(palettePath)) {
        std::cerr << "Failed to load palette" << std::endl;
        return 1;
    }

    // Get all distinct colors
    std::cout << "Pixels:\t" << width * height << std::endl;
    std::unordered_map<uint32_t, glm::vec3> colors;
    for (size_t i = 0; i < width * height; i++) {
        uint8_t* color = data + (i * channels);
        uint32_t key = color_key(color[0], color[1], color[2]);
        colors.insert({key, color_vec(key)});
    }
    std::cout << "Colors:\t" << colors.size() << std::endl;

    // Map to desired color space
    std::unique_ptr<PipelineStep> pipeline = std::make_unique<NullStep>();

    switch (space) {
    case ColorSpace::Rgb: break;
    case ColorSpace::Xyz: {
        pipeline->append(std::make_unique<XyzConversion>());
        break;
    }
    case ColorSpace::Lab50: {
        pipeline->append(std::make_unique<XyzConversion>());
        pipeline->append(std::make_unique<LabConversion>(LabConversion::D50));
        break;
    }
    case ColorSpace::Lab65: {
        pipeline->append(std::make_unique<XyzConversion>());
        pipeline->append(std::make_unique<LabConversion>(LabConversion::D65));
        break;
    }
    }

    std::vector<glm::vec3> mapped(palette.colors());
    std::transform(mapped.begin(), mapped.end(), mapped.begin(), [&pipeline](glm::vec3 v) {
        return pipeline->execute(v);
    });

    switch (algorithm) {
    case Algorithm::ClosestEuclid: {
        pipeline->append(std::make_unique<ClosestEuclidian>(mapped));
        break;
    }

    case Algorithm::ClosestLine: {
        pipeline->append(std::make_unique<ClosestLine>(mapped));
        break;
    }
        /*
case Algorithm::Blend: {
using namespace std::placeholders;
colorSelector = std::make_unique<PartitionBlend>(
palette, std::bind(brightness, channel, _1, _2), colorSpace.get()
);
break;
}
*/
    default: {
        std::cerr << "Unsupported algorithm" << std::endl;
        return 1;
    }
    }

    switch (space) {
    case ColorSpace::Rgb: break;
    case ColorSpace::Xyz: {
        pipeline->append(std::make_unique<XyzConversion>(true));
        break;
    }
    case ColorSpace::Lab50: {
        pipeline->append(std::make_unique<LabConversion>(LabConversion::D50, true));
        pipeline->append(std::make_unique<XyzConversion>(true));
        break;
    }
    case ColorSpace::Lab65: {
        pipeline->append(std::make_unique<LabConversion>(LabConversion::D65, true));
        pipeline->append(std::make_unique<XyzConversion>(true));
        break;
    }
    }

    for (auto& [key, value] : colors) {
        colors[key] = pipeline->execute(value);
    }

    // Apply mapping to image
    for (size_t i = 0; i < width * height; i++) {
        uint8_t* color = data + (i * channels);
        uint32_t key = color_key(color[0], color[1], color[2]);
        glm::vec3 mapped = colors[key];
        mapped = glm::clamp(mapped, {0, 0, 0}, {1, 1, 1});
        color[0] = mapped.r * 255;
        color[1] = mapped.g * 255;
        color[2] = mapped.b * 255;
    }

    // Write image to file
    int result = stbi_write_jpg(outputPath.c_str(), width, height, channels, data, 100);
    if (result == 0) {
        std::cerr << "Failed to write image: " << stbi_failure_reason() << std::endl;
        return 1;
    }
}

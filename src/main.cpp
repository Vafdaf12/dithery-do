#include "Palette.h"
#include "pipeline/LabConversion.h"
#include "pipeline/PipelineStep.h"
#include "pipeline/XyzConversion.h"
#include "select/ClosestEuclidian.h"
#include "select/ClosestLine.h"
#include "select/PartitionBlend.h"

#include "argparse/argparse.hpp"
#include "glm/common.hpp"
#include "glm/geometric.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

enum class Algorithm : int {
    ClosestEuclid = 0,
    ClosestLine,
    DotBlend,
};
std::pair<std::unique_ptr<PipelineStep>, std::unique_ptr<PipelineStep>>
createColorSpace(const std::string& val) {
    if (val == "rgb") {
        return {
            std::make_unique<NullStep>(),
            std::make_unique<NullStep>(),
        };
    } else if (val == "xyz") {
        return {
            std::make_unique<XyzConversion>(),
            std::make_unique<XyzConversion>(true),
        };
    } else if (val == "lab50") {
        auto from = std::make_unique<XyzConversion>();
        from->append(std::make_unique<LabConversion>(LabConversion::D50));

        auto to = std::make_unique<LabConversion>(LabConversion::D50, true);
        to->append(std::make_unique<XyzConversion>(true));
        return {std::move(from), std::move(to)};
    } else if (val == "lab65") {
        auto from = std::make_unique<XyzConversion>();
        from->append(std::make_unique<LabConversion>(LabConversion::D65));

        auto to = std::make_unique<LabConversion>(LabConversion::D65, true);
        to->append(std::make_unique<XyzConversion>(true));
        return {std::move(from), std::move(to)};
    } else {
        throw std::invalid_argument("Unsupported color space: " + val);
    }
}

Algorithm algo_from_string(const std::string& val) {
    if (val == "euclid") {
        return Algorithm::ClosestEuclid;
    } else if (val == "line") {
        return Algorithm::ClosestLine;
    } else if (val == "dot_blend") {
        return Algorithm::DotBlend;
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

int main(int argc, char** argv) {
    argparse::ArgumentParser cli("Dithery Do");

    cli.add_argument("input").help("The input image");
    cli.add_argument("palette").help("Path to palette file");

    cli.add_argument("-o").help("Path of output image").default_value("output.jpg");

    cli.add_argument("-a", "-algo")
        .help("The color selection algorithm to use")
        .choices("euclid", "line", "dot_blend")
        .required();

    cli.add_argument("-s", "-space")
        .help("The color space to use for selection")
        .choices("rgb", "xyz", "lab50", "lab65")
        .default_value("rgb")
        .required();

    cli.add_argument("-v", "--vector")
        .help("The direction vector to use for dot blend")
        .nargs(3)
        .default_value(std::vector<float>{1, 0, 0})
        .scan<'g', float>();

    // Parse the output
    std::string inputPath;
    std::string outputPath;
    std::string palettePath;

    Algorithm algorithm;
    std::pair<std::unique_ptr<PipelineStep>, std::unique_ptr<PipelineStep>> colorSpace;
    glm::vec3 vec;

    try {
        cli.parse_args(argc, argv);
        colorSpace = createColorSpace(cli.get("-space"));
        algorithm = algo_from_string(cli.get("-algo"));
        inputPath = validate_path(cli.get("input"));
        palettePath = validate_path(cli.get("palette"));
        outputPath = cli.get("-o");
        auto vals = cli.get<std::vector<float>>("-v");
        vec = {vals[0], vals[1], vals[2]};
        vec = glm::normalize(vec);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << cli;
        return 1;
    }

    std::cout << "Read File: " << inputPath << std::endl;
    std::cout << "Write File: " << outputPath << std::endl;
    std::cout << "Palette: " << palettePath << std::endl;
    std::cout << "Algorithm: " << int(algorithm) << std::endl;

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
    std::unique_ptr<PipelineStep> pipeline = std::move(colorSpace.first);

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
    case Algorithm::DotBlend: {
        pipeline->append(std::make_unique<PartitionBlend>(mapped, vec));
        break;
    }
    }

    pipeline->append(std::move(colorSpace.second));

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

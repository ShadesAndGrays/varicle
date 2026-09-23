#pragma once

#include "vendor/stb/stb_image.h"

#include <exception>
#include <expected>
#include <fstream>
#include <optional>
#include <print>

namespace varicle::renderer::opengl {

using std::println;

using StbImagePtr = std::unique_ptr<unsigned char, decltype(&stbi_image_free)>;

struct ImageData {

    int         w, h, c;
    StbImagePtr data;
};

inline std::optional<std::string> read_file(std::string file_path) {
    std::fstream file(file_path, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        println("Failed to load file: {}", file_path);
        return std::nullopt;
    }

    auto size = file.tellg();
    file.seekg(std::ios::beg);

    std::string result;
    result.resize(size);

    file.read(result.data(), size);

    return result;
}

inline std::expected<ImageData, std::string> load_image(const std::string& file_path) {

    std::fstream file(file_path, std::ios::in);
    if (!file.is_open()) {
        return std::unexpected<std::string>(
            format("Failed to load image from path: {}", file_path)
        );
    }

    try {
        int            w, h, channels;
        unsigned char* pixels =
            stbi_load(file_path.c_str(), &w, &h, &channels, 4);
        return ImageData{
            w, h, channels, StbImagePtr(pixels, stbi_image_free)
        };
    } catch (std::exception) {
        return std::unexpected<std::string>("Error occured when loading image");
    }
}

}

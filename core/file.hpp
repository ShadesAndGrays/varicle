#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#pragma once

namespace varicle::core {

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

} // namespace varicle::core

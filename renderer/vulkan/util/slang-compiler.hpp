#pragma once

#include <print>
#include <vector>
using std::println;

struct CompiledShader {
    std::string           entry_point;
    std::vector<uint32_t> code;
};

std::vector<CompiledShader> compile_shader_code(
    std::string              path,
    std::vector<std::string> entry_point_names,
    std::vector<std::string> include_dirs = {}
);

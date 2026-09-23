#pragma once

#include "renderer/common/handles.hpp"

#include <glm/glm.hpp>
#include <string>

namespace varicle::renderer::opengl {
struct Shader {

    renderer::ShaderHandle ID;

    void use();
    void set(const std::string& property, float value);
    void set(const std::string& property, const std::array<float, 2>& value);
    void set(const std::string& property, const std::array<float, 3>& value);
    void set(const std::string& property, const std::array<float, 4>& value);
    void set(const std::string& property, const glm::mat4& value);
};
} // namespace varicle::renderer::opengl

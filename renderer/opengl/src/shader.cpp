#include "renderer/opengl/shader.hpp"

#include <glad/glad.h>

#include <expected>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>

using std::println;
using namespace varicle::renderer::opengl;

void Shader::use() {
    glad_glUseProgram(ID);
}

std::expected<int, std::string>
get_property_location(uint32_t ID, const std::string& property) {
    auto loc = glad_glGetUniformLocation(ID, property.c_str());
    if (loc == -1) {
        auto err = std::format("Property {}does not exist in shader", property);
        println("{}", err);
        return std::unexpected<std::string>(err);
    }
    return loc;
}

void Shader::set(const std::string& property, float value) {
    auto _ = get_property_location(ID, property).transform([value](auto loc) {
        glad_glUniform1f(loc, value);
    });
}

void Shader::set(
    const std::string&          property,
    const std::array<float, 2>& value
) {
    auto _ = get_property_location(ID, property).transform([value](auto loc) {
        glad_glUniform2fv(loc, 1, value.data());
    });
}

void Shader::set(
    const std::string&          property,
    const std::array<float, 3>& value
) {
    auto _ = get_property_location(ID, property).transform([value](auto loc) {
        glad_glUniform3fv(loc, 1, value.data());
    });
}

void Shader::set(
    const std::string&          property,
    const std::array<float, 4>& value
) {
    auto _ = get_property_location(ID, property).transform([value](auto loc) {
        glad_glUniform4fv(loc, 1, value.data());
    });
}

void Shader::set(const std::string& property, const glm::mat4& value) {

    auto _ = get_property_location(ID, property).transform([value](auto loc) {
        glad_glUniformMatrix4fv(loc, 1,GL_FALSE, glm::value_ptr(value));
    });
}

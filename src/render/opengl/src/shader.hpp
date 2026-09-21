#pragma once
#include <glm/glm.hpp>
#include <string>

struct Shader {
    uint32_t ID;

    void use();
    void set(const std::string& property, float value);
    void set(const std::string& property, const std::array<float, 2>& value);
    void set(const std::string& property, const std::array<float, 3>& value);
    void set(const std::string& property, const std::array<float, 4>& value);
    void set(const std::string& property, const glm::mat4& value);

};

#pragma once

#include "handles.hpp"
#include <glm/glm.hpp>

namespace varicle::renderer {

// Strucutre for managing mesh level transfromations
// Stick with simple oop for now consider SOA for future optimizations
struct Object {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

    MeshHandle     mesh;
    TextureHandle  texture;
    MaterialHandle material;
    ShaderHandle   shader;

    glm::mat4 get_model_matrix() const;
};

} // namespace varicle::renderer

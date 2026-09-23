#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace varicle::renderer::opengl {
struct Transform {
    float position[3];
    float scale[3];
    float rotation[3];
};

glm::mat4 get_model_matrix(const Transform& transform);

} // namespace varicle::renderer::opengl

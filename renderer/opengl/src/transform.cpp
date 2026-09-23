#include "renderer/opengl/transform.hpp"

using namespace varicle::renderer::opengl;

glm::mat4 get_model_matrix(const Transform& transform) {

    const auto& [x, y, z]    = transform.position;
    const auto& [w, h, d]    = transform.scale;
    const auto& [rx, ry, rz] = transform.rotation;

    glm::vec3 translate_vector(x, y, z);
    glm::vec3 scale_vector(w, h, d);

    glm::mat4 translate_matrix = glm::translate(translate_vector);
    glm::mat4 scale_matrix     = glm::scale(scale_vector);
    glm::mat4 rotation_matrix(1);

    rotation_matrix = glm::rotate(rotation_matrix, rx, { 1, 0, 0 });
    rotation_matrix = glm::rotate(rotation_matrix, ry, { 0, 1, 0 });
    rotation_matrix = glm::rotate(rotation_matrix, rz, { 0, 0, 1 });

    auto      rot_scale = rotation_matrix * scale_matrix;
    glm::mat4 model     = translate_matrix * rot_scale;

    return model;
}

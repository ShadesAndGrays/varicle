
#include "common.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

glm::mat4 varicle::render::Object::get_model_matrix() const {
    glm::mat4 model = glm::mat4(1.0f); // identity
    model           = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}
glm::mat4 varicle::render::Camera::get_projection_matix() {
    if (projection_type == PERSPECTIVE) {
        auto proj = glm::perspective(glm::radians(45.0f), aspect, near, far);
        proj[1][1] *= -1;
        return proj;
    } else
        return glm::perspective(glm::radians(45.0f), aspect, near, far);
}
glm::mat4 varicle::render::Camera::get_view_matrix() {
    return glm::lookAt(position, target, up);
}

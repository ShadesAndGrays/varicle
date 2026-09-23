
#include "renderer/common/common.hpp"
#include "renderer/common/camera.hpp"
#include "renderer/common/object.hpp"
#include "renderer/common/transform.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <print>

varicle::renderer::Window::Window(int width, int height, const char* title)
    : m_initial_width(width), m_initial_height(height),
      m_initial_title(std::move(title)) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
}

GLFWwindow* varicle::renderer::Window::get_window() {
    return m_window;
}

std::tuple<int, int> varicle::renderer::Window::size() {
    int width;
    int height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return { width, height };
}
bool varicle::renderer::Window::should_close_window() {
    return glfwWindowShouldClose(m_window);
}

float varicle::renderer::Window::get_aspect() {
    auto [w, h] = size();
    return static_cast<float>(w) / static_cast<float>(h);
}

// glm::mat4 varicle::renderer::Object::get_model_matrix() const {
//     glm::mat4 model = glm::mat4(1.0f); // identity
//     model           = glm::translate(model, position);
//     model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//     model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//     model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//     model = glm::scale(model, scale);
//
//     return model;
// }
// glm::mat4 varicle::renderer::Camera::get_projection_matix() {
//     if (projection_type == PERSPECTIVE) {
//         auto proj = glm::perspective(glm::radians(45.0f), aspect, near, far);
//         proj[1][1] *= -1;
//         return proj;
//     } else
//         return glm::perspective(glm::radians(45.0f), aspect, near, far);
// }
//
// glm::mat4 varicle::renderer::Camera::get_view_matrix() {
//     return glm::lookAt(position, target, up);
// }


varicle::renderer::Mesh varicle::renderer::get_primitive(varicle::renderer::PrimitiveType type) {
    using namespace varicle::renderer;

    switch (type) {
    case TRIANGLE:
        /*
         0
        / \
        2-1
         */
        return Mesh{ {
                         { 0.0, 0.5, 0, 0.0, 0.0, 0.0, 0.0, 0.5 },
                         { 0.5, -0.5, 0, 0.0, 0.0, 0.0, 0.5, -0.5 },
                         { -0.5, -0.5, 0, 0.0, 0.0, 0.0, -0.5, -0.5 },
                     },
                     { 0, 1, 2} };
    case QUAD:

        /*
         0---1
         |   |
         3 --2
         */
        return Mesh {
            {
                       { -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                       { 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                       { 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
                       { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f } },
                     { 0, 1, 2, 0, 2, 3 } };
    case TWO_QUAD_CUBE:
        /* front quad

         0---1 ... 4---5
         | f |     | b |
         3 --2 ... 7 --6
               + z -

        back quad */
        return Mesh{
            {
                { -0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0 },
                { 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 0 },
                { 0.5, -0.5, 0.5, 0.0, 0.0, 0.0, 1.0, 1.0 },
                { -0.5, -0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0 },

                { -0.5, 0.5, -0.5, 0.0, 0.0, 0.0, 0.0, 0.0 },
                { 0.5, 0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0 },
                { 0.5, -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0 },
                { -0.5, -0.5, -0.5, 0.0, 0.0, 0.0, 0.0, 1.0 },
            },
            {
                0,1,2, 0,2,3,
                4,5,6, 4,6,7,
                1,0,4, 1, 4,5,
                1,5,6, 1, 6,2,
                3,2,6, 3, 6,7,
                4,0,3, 4, 3,7,
                     } };
    case FULLSCREEN_TRIANGLE:
        /*
         0
         | \
         2--1
*/
        return { {
                     { -1.0, 3.0, 0, 0.0, 0.0, 0.0, -1.0, 3.0 },
                     { 3.0, -1.0, 0, 0.0, 0.0, 0.0, 3.0, -1.0 },
                     { -1.0, -1.0, 0, 0.0, 0.0, 0.0, -1.0, -1.0 },
                 },
                 {0,1,2} };
    default:
        return {};
    }


}


glm::mat4 varicle::renderer::get_model_matrix(const Transform& transform) {

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


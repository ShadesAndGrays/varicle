
#include "renderer/common/common.hpp"
#include "renderer/common/camera.hpp"
#include "renderer/common/object.hpp"
#include "renderer/common/window.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

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

glm::mat4 varicle::renderer::Object::get_model_matrix() const {
    glm::mat4 model = glm::mat4(1.0f); // identity
    model           = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}
glm::mat4 varicle::renderer::Camera::get_projection_matix() {
    if (projection_type == PERSPECTIVE) {
        auto proj = glm::perspective(glm::radians(45.0f), aspect, near, far);
        proj[1][1] *= -1;
        return proj;
    } else
        return glm::perspective(glm::radians(45.0f), aspect, near, far);
}
glm::mat4 varicle::renderer::Camera::get_view_matrix() {
    return glm::lookAt(position, target, up);
}


#include "common.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

void varicle::render::Window::init(int width, int height, const char* title) {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

GLFWwindow* varicle::render::Window::get_window() {
    return m_window;
}

std::tuple<int, int> varicle::render::Window::size() {
    int width;
    int height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return { width, height };
}
bool varicle::render::Window::should_close_window() {
    return glfwWindowShouldClose(m_window);
}

float varicle::render::Window::get_aspect() {
    auto [w, h] = size();
    return static_cast<float>(w) / static_cast<float>(h);
}

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

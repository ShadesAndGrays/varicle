
#include "renderer/common/common.hpp"
#include "renderer/common/camera.hpp"
// #include "renderer/common/object.hpp"
#include "renderer/common/transform.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"

#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

varicle::renderer::Window::Window(
    int             width,
    int             height,
    const char*     title,
    GraphicsBackend backend
)
    : m_initial_width(width), m_initial_height(height),
      m_initial_title(std::move(title)) {

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if (backend == GraphicsBackend::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    } else if (backend == GraphicsBackend::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    } else {
    }
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

glm::mat4 varicle::renderer::Camera::get_projection_matix(
    varicle::renderer::GraphicsBackend backend
) const {
    if (type == CameraType::PERSPECTIVE) {
        if (backend == GraphicsBackend::Vulkan) {
            auto proj =
                glm::perspectiveRH_ZO(glm::radians(fov), aspect(), near, far);
            proj[1][1] *= -1;
            return proj;
        }
        return glm::perspectiveRH_NO(glm::radians(fov), aspect(), near, far);
    } else {
        if (backend == GraphicsBackend::Vulkan) {
            auto proj = glm::orthoRH_ZO(
                -width / 2, width / 2, -height / 2, height / 2, near, far
            );
            proj[1][1] *= -1;
            return proj;
        }
        return glm::orthoRH_NO(
            -width / 2, width / 2, -height / 2, height / 2, near, far
        );
    }
}

glm::mat4 varicle::renderer::Camera::get_view_matrix() const {
    return glm::lookAt(
        glm::vec3(position.x, position.y, position.z),
        glm::vec3(target.x, target.y, target.z),
        glm::vec3(up.x, up.y, up.z)
    );
}

glm::mat4 varicle::renderer::Camera::get_view_project_matrix(
    varicle::renderer::GraphicsBackend backend
) const {
    return get_projection_matix(backend) * get_view_matrix();
}
varicle::renderer::Mesh
varicle::renderer::get_primitive(varicle::renderer::PrimitiveType type) {
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
                     { 0, 1, 2 } };
    case QUAD:

        /*
         0---1
         |   |
         3 --2
         */
        return Mesh{ { { -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                       { 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
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
        return Mesh{ {
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
                         0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 1, 0, 4, 1, 4, 5,
                         1, 5, 6, 1, 6, 2, 3, 2, 6, 3, 6, 7, 4, 0, 3, 4, 3, 7,
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
                 { 0, 1, 2 } };
    default:
        return {};
    }
}

glm::mat4 varicle::renderer::Transform::get_model_matrix() const {

    const auto [x, y, z]    = position.v;
    const auto [w, h, d]    = scale.v;
    const auto [rx, ry, rz] = rotation.v;

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

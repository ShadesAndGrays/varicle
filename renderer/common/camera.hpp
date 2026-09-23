#pragma once

#include <glm/glm.hpp>

namespace varicle::renderer {

struct Camera {

    enum PROJECTION_TYPE { PERSPECTIVE, ORTHOGRAPHIC };
    glm::vec3       position{ 8.0f, 8.0f, 8.0f };
    glm::vec3       target{ 0.0f, 0.0f, 0.0f };
    glm::vec3       up{ 0.0f, 0.0f, 1.0f };
    PROJECTION_TYPE projection_type = PERSPECTIVE;

    float near   = 0.1f;
    float far    = 20.0f;
    float fov    = 45.0f;
    float aspect = 1;

    glm::mat4 get_projection_matix();
    glm::mat4 get_view_matrix();
};

} // namespace varicle::renderer

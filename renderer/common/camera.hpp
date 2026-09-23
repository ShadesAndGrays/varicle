#pragma once

#include <glm/glm.hpp>

namespace varicle::renderer {

struct Camera {

    enum CameraType {
        PERSPCTIVE,
        ORTHOGRAHIC,
    };

    CameraType type = ORTHOGRAHIC;

    float width;
    float height;

    // Projection: Perspective
    float near;
    float far;

    // Projection: Perspective
    float fov;

    // Projection: View
    float center[3];
    float eye[3];
    float up[3];

    float aspect() const { return width / height; }

    static Camera default_orthographic() {
        return {
            CameraType::ORTHOGRAHIC, 1280.f,      720.f,      0.1f, 100.f, 0.0f,
            { 0, 0, -10 },           { 0, 0, 0 }, { 0, 1, 0 }
        };
    }

    static Camera default_perspective() {
        return {
            CameraType::PERSPCTIVE, 1280.f,      720.f,      -1.0f, 1.f, 45.f,
            { 0, 0, -10 },          { 0, 0, 0 }, { 0, 1, 0 }
        };
    }

    glm::mat4 get_projection_matix();
    glm::mat4 get_view_matrix();
};

} // namespace varicle::renderer

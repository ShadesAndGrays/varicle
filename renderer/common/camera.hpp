#pragma once

#include "core/type.hpp"
#include "renderer/common/window.hpp"
#include <glm/glm.hpp>

namespace varicle::renderer {

struct Camera {

    enum CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC,
    };

    CameraType type = ORTHOGRAPHIC;

    float width;
    float height;

    // Projection
    float near;
    float far;

    // Projection: Perspective
    float fov;

    // Projection: View
    Vec3 position{ 0, 0, 0 };
    Vec3 target{ 0, 0, 0 };
    Vec3 up{ 0, 1, 0 };

    float aspect() const { return width / height; }

    static Camera default_orthographic() {
        return { CameraType::ORTHOGRAPHIC,
                 1280.f,
                 720.f,
                 0.1f,
                 100.f,
                 0.0f,
                 { 0, 0, -1 },
                 { 0, 0, 0 },
                 { 0, 1, 0 } };
    }

    static Camera default_perspective() {
        return { CameraType::PERSPECTIVE,
                 1280.f,
                 720.f,
                 0.1f,
                 100.0f,
                 45.f,
                 { 10, 10, 10 },
                 { 0, 0, 0 },
                 { 0, 1, 0 } };
    }

    glm::mat4 get_projection_matix(
        GraphicsBackend backend = GraphicsBackend::OpenGL
    ) const;
    // glm::mat4 get_view_matrix() const;
    Mat4      get_view_matrix() const;
    glm::mat4 get_view_project_matrix(
        GraphicsBackend backend = GraphicsBackend::OpenGL
    ) const;
};

} // namespace varicle::renderer

#pragma once

#include "core/type.hpp"
#include "core/constants.hpp"
#include "renderer/common/window.hpp"

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

    // Field of view angle in radians
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
                 45.f * varicle::constants::deg_to_rad,
                 { 10, 10, 10 },
                 { 0, 0, 0 },
                 { 0, 1, 0 } };
    }

    Mat4 get_projection_matix(
        GraphicsBackend backend = GraphicsBackend::OpenGL
    ) const;
    Mat4 get_view_matrix() const;
    Mat4 get_view_project_matrix(
        GraphicsBackend backend = GraphicsBackend::OpenGL
    ) const;
};

} // namespace varicle::renderer

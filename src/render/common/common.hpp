#pragma once

#include "handles.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>
// Forward decleration

// Strucutre for managing mesh level transfromations
// Stick with simple oop for now consider SOA for future optimizations

class GLWindow {
  private:
    GLFWwindow* window;
};

namespace varicle::render {

// Strucutre for managing mesh level transfromations
// Stick with simple oop for now consider SOA for future optimizations
struct Object {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

    MeshHandle     mesh;
    TextureHandle  texture;
    MaterialHandle material;

    glm::mat4 get_model_matrix() const;
};

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

struct Color {

    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

struct Rect {
    float x, y, width, height;
};

class IRender {
  public:
    virtual void    set_camera(Camera camera) = 0;
    virtual Camera& get_camera() = 0;
    virtual ~IRender() = default;

    [[deprecated(
        "This should be removed as user should not need access to window "
        "outside render"
    )]] virtual GLFWwindow*
    get_window() = 0;

    // Lifecycle
    virtual void
    init(uint32_t width, uint32_t height, const char* window_name) = 0;
    virtual void shutdown()                                        = 0;
    virtual void resieze(uint32_t width, uint32_t height)          = 0;
    virtual bool should_close_window()                             = 0;

    // Frame Management
    virtual void set_clear_color(Color background)     = 0;
    virtual void begin_frame(bool clear_screen = true) = 0;
    virtual void end_frame()                           = 0;

    // Texture Managment
    virtual TextureHandle load_texture(const char* filepath)     = 0;
    virtual void          destroy_texture(TextureHandle texture) = 0;

    // Mesh Managment
    virtual MeshHandle load_mesh(const char* filepath) = 0;
    virtual void       destroy_mesh(MeshHandle mesh)   = 0;

    // Drawing Command
    virtual void draw_mesh(
        glm::vec3      position,
        glm::vec3      rotation,
        glm::vec3      scale,
        MeshHandle     mesh     = INVALID_MESH,
        TextureHandle  texture  = INVALID_TEXTURE,
        MaterialHandle material = INVALID_MATERIAL
    ) = 0;

    virtual void draw_mesh(MeshHandle mesh)                      = 0;
    virtual void draw_rect(const Rect& rect, const Color& color) = 0;

    virtual void draw_texured_rect(
        const Rect&   rect,
        TextureHandle texture,
        const Color&  tint = { 1, 1, 1, 1 }
    ) = 0;
};

} // namespace varicle::render

#pragma once

#include "handles.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>

namespace varicle::render {
struct Color {

    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

struct Rect {
    float x, y, width, height;
};

class IRender {
  public:
    virtual ~IRender() = default;

    virtual GLFWwindow* get_window() = 0;

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
    virtual void draw_mesh(MeshHandle mesh) = 0;
    virtual void draw_rect(const Rect& rect, const Color& color) = 0;

    virtual void draw_texured_rect(
        const Rect&   rect,
        TextureHandle texture,
        const Color&  tint = { 1, 1, 1, 1 }
    ) = 0;
};

} // namespace varicle::render

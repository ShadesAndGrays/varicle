#pragma once
#include "common.hpp"
#include "graphics/resource-manager.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {

class VulkanRenderer : public IRender {

  public:
    GLFWwindow* get_window() override;
    bool        should_close_window() override;

    void

    init(uint32_t width, uint32_t height, const char* window_name) override;

    void shutdown() override;

    void begin_frame(bool clear_screen = true) override;
    void set_clear_color(Color background) override;

    void end_frame() override;

    void resieze(uint32_t width, uint32_t height) override {}

    TextureHandle load_texture(const char* filepath) override;
    void          destroy_texture(TextureHandle texture) override;

    TextureHandle load_mesh(const char* filepath) override;
    void          destroy_mesh(MeshHandle mesh) override;

    void draw_rect(const Rect& rect, const Color& color) override;

    void draw_mesh(MeshHandle mesh) override;

    void draw_v_cube();

    void draw_texured_rect(
        const Rect&   rect,
        TextureHandle texture,
        const Color&  tint = { 1, 1, 1, 1 }
    ) override {}

  private:
    struct Impl;
    ResourceManager resource_manager;
    Impl*           impl = nullptr;
};

} // namespace varicle::render::vulkan

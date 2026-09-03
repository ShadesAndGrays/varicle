#pragma once
#include "common.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {

class VulkanRenderer : public IRender {

  public:
    bool should_close_window() override;

    void

    init(uint32_t width, uint32_t height, const char* window_name) override;

    void shutdown() override;

    void begin_frame(bool clear_screen = true) override;
    void set_clear_color(Color background) override;

    void end_frame() override;

    void resieze(uint32_t width, uint32_t height) override {}

    TextureHandle create_texture(const char* filepath) override {
        return INVALID_TEXTURE;
    }

    void destroy_texture(TextureHandle texture) override {}

    void draw_rect(const Rect& rect, const Color& color) override {}

    void draw_texured_rect(
        const Rect&   rect,
        TextureHandle texture,
        const Color&  tint = { 1, 1, 1, 1 }
    ) override {}

  private:
    struct Impl;
    Impl* impl = nullptr;
};

} // namespace varicle::render::vulkan

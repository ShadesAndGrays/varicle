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

    void begin_frame() override;
    void clear_color(Color background) override;

    void end_frame() override;

    void draw_circle(float x, float y, float radius, Color color) override {
        // TODO: Implement this pure virtual method.
        // static_assert(false, "Method `draw_circle` is not implemented.");
    }

    void draw_rect(
        float x,
        float y,
        float widht,
        float height,
        Color color
    ) override {
        // TODO: Implement this pure virtual method.
        // static_assert(false, "Method `draw_rect` is not implemented.");
    }

  private:
    struct Impl;
    Impl* impl = nullptr;
};

} // namespace varicle::render::vulkan

#pragma once
#include "common.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {


class VulkanRenderer : public IRender {

  public:
    bool should_close_window() override;

    void
    init(void* native_window_handle, uint32_t width, uint32_t height) override;

    void shutdown() override; 

    void begin_frame() override {
        // TODO: Implement this pure virtual method.
        // static_assert(false, "Method `begin_frame` is not implemented.");
    }

    void end_frame() override {
        // TODO: Implement this pure virtual method.
        // static_assert(false, "Method `end_frame` is not implemented.");
    }

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

} // namespace varicle::render

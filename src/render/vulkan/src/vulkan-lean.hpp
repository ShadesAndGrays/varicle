#include "common.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render {

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

class VulkanRenderer : public IRender {

  public:
    void
    init(void* native_window_handle, uint32_t width, uint32_t height) override;

    void shutdown() override {
        // TODO: Implement this pure virtual method.
        // static_assert(false, "Method `shutdown` is not implemented.");
    }

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
};

} // namespace varicle::render


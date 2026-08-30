#include "vulkan-renderer.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "core/device.hpp"
#include "core/impl.hpp"
#include "core/init.hpp"
#include "core/swap-chain.hpp"
#include "core/validation.hpp"

#include <print>

using namespace varicle::render::vulkan;

void VulkanRenderer ::init(
    uint32_t    width,
    uint32_t    height,
    const char* window_name

) {
    impl = new Impl{};

    std::println("VulkanRender: Initializing Vulkan");

    VulkanContext& ctx = impl->v_context;
    init_window(ctx, width, height, window_name);
    create_instance(ctx);
    if (enable_validation_layers)
        debug::setupDebugMessenger(ctx);
    create_surface(ctx);
    select_physical_device(ctx);
    create_logical_device(ctx);
    create_swap_chain(ctx);
}

bool VulkanRenderer::should_close_window() {
    return glfwWindowShouldClose(impl->v_context.m_window);
}

void VulkanRenderer::shutdown() {
    std::println("Shutting down....");
    auto& ctx = impl->v_context;

    ctx.m_device.destroy();
    ctx.m_instance.destroySurfaceKHR(ctx.m_surface);
#if !defined(NDEBUG)
    ctx.m_instance.destroyDebugUtilsMessengerEXT(ctx.m_debugMessenger);
#endif
    ctx.m_instance.destroy();

    glfwDestroyWindow(impl->v_context.m_window);
    glfwTerminate();

    // call last
    if (impl != nullptr)
        delete impl;
}

void VulkanRenderer::begin_frame() {}

void VulkanRenderer::clear_color(Color background) {}

void VulkanRenderer::end_frame() {
    glfwPollEvents();
}

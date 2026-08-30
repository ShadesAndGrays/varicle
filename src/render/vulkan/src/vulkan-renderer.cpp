#include "vulkan-renderer.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "core/device.hpp"
#include "core/impl.hpp"
#include "core/init.hpp"
#include "core/swap-chain.hpp"
#include "core/validation.hpp"
#include "core/vertex.hpp"
#include "graphics/buffer.hpp"
#include "graphics/pipeline.hpp"
#include "util/image-util.hpp"

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
    create_image_views(ctx);
    create_graphics_pipeline(ctx);
    create_command_pool(ctx);
    create_vertex_buffer(ctx);
    create_command_buffers(ctx);
    create_sync_objects(ctx);
}

bool VulkanRenderer::should_close_window() {
    return glfwWindowShouldClose(impl->v_context.m_window);
}

void VulkanRenderer::shutdown() {

    auto& ctx = impl->v_context;
    ctx.m_device.waitIdle();

    std::println("Shutting down....");
    for (auto& i : ctx.m_present_complete_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_render_finished_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_in_flight_fences)
        ctx.m_device.destroyFence(i);
    ctx.m_device.destroyShaderModule(ctx.m_shader_module);
    ctx.m_device.freeMemory(ctx.m_vertex_buffer_memory);
    ctx.m_device.destroyBuffer(ctx.m_vertex_buffer);
    ctx.m_device.destroyPipeline(ctx.m_graphics_pipeline);
    ctx.m_device.destroyPipelineLayout(ctx.m_pipeline_layout);
    ctx.m_device.destroyCommandPool(ctx.m_command_pool);
    for (auto& view : ctx.m_swap_chain_image_views)
        ctx.m_device.destroyImageView(view);
    ctx.m_device.destroySwapchainKHR(ctx.m_swap_chain);
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

void VulkanRenderer::clear_color(Color background) {
    auto& ctx         = impl->v_context;
    ctx.m_clear_color = vk::ClearColorValue(
        background.r, background.g, background.b, background.a
    );
}

void VulkanRenderer::begin_frame() {
    auto& ctx = impl->v_context;
    ctx.m_recreating_frame = false;
    

    auto fence_result = ctx.m_device.waitForFences(
        1, &ctx.m_in_flight_fences[ctx.m_frame_index], vk::True, UINT64_MAX
    );

    if (fence_result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fence!");
    }

    auto [result, image_index] = ctx.m_device.acquireNextImageKHR(
        ctx.m_swap_chain,
        UINT64_MAX,
        ctx.m_present_complete_semaphores[ctx.m_frame_index],
        nullptr
    );

    ctx.m_image_index = image_index;

    if (result == vk::Result::eErrorOutOfDateKHR) {
        std::println("Swap chain no longer compatible! Recreating swap chain");
        recreate_swap_chain(ctx);
        ctx.m_recreating_frame = true;
        return;
    }

    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR) {
        assert(
            result == vk::Result::eTimeout || result == vk::Result::eNotReady
        );
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // re-enable fence for next frame
    ctx.m_device.resetFences(ctx.m_in_flight_fences[ctx.m_frame_index]);

    vk::CommandBuffer cmd = ctx.m_command_buffers[ctx.m_frame_index];
    cmd.reset();
    vk::CommandBufferBeginInfo begin_info{};
    result = cmd.begin(&begin_info);

    transition_image_layout(
        ctx,
        image_index,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput
    );

    vk::ClearValue              clearColor     = ctx.m_clear_color;
    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView   = ctx.m_swap_chain_image_views[image_index],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = clearColor
    };

    vk::RenderingInfo rendering_info = {
        .renderArea = { .offset = { 0, 0 }, .extent = ctx.m_swap_chain_extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &attachmentInfo
    };

    cmd.beginRendering(rendering_info);
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.m_graphics_pipeline);

    cmd.bindVertexBuffers(0, ctx.m_vertex_buffer, { 0 });

    cmd.setViewport(
        0,
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(ctx.m_swap_chain_extent.width),
            static_cast<float>(ctx.m_swap_chain_extent.height),
            0.0f,
            1.0f
        )
    );
    cmd.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), ctx.m_swap_chain_extent));

    cmd.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}

void VulkanRenderer::end_frame() {

    auto& ctx = impl->v_context;
    if(ctx.m_recreating_frame) return;
    ctx.m_command_buffers[ctx.m_frame_index].endRendering();
    const auto imageIndex = ctx.m_image_index;

    transition_image_layout(
        ctx,
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe
    );

    ctx.m_command_buffers[ctx.m_frame_index].end();

    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );
    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores =
            &ctx.m_present_complete_semaphores[ctx.m_frame_index],
        .pWaitDstStageMask    = &waitDestinationStageMask,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &ctx.m_command_buffers[ctx.m_frame_index],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &ctx.m_render_finished_semaphores[imageIndex]
    };

    ctx.m_graphics_queue.submit(
        submitInfo, ctx.m_in_flight_fences[ctx.m_frame_index]
    );

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &ctx.m_render_finished_semaphores[imageIndex],
        .swapchainCount     = 1,
        .pSwapchains        = &ctx.m_swap_chain,
        .pImageIndices      = &imageIndex
        // presentInfoKHR.pResults = nullptr;
    };

    auto result = ctx.m_present_queue.presentKHR(presentInfoKHR);

    if (result == vk::Result::eSuboptimalKHR ||
        result == vk::Result::eErrorOutOfDateKHR || ctx.m_framebuffer_resized) {
        ctx.m_framebuffer_resized = false;
        recreate_swap_chain(ctx);
    } else {
        assert(result == vk::Result::eSuccess);
    }

    ctx.m_frame_index = (ctx.m_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;

    glfwPollEvents();
}

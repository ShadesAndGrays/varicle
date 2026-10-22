#include "renderer/vulkan/lib.hpp"
#include "core/math.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/core/device.hpp"
#include "renderer/vulkan/core/init.hpp"
#include "renderer/vulkan/core/swap-chain.hpp"
#include "renderer/vulkan/core/validation.hpp"
#include "renderer/vulkan/graphics/buffer.hpp"
#include "renderer/vulkan/graphics/image.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource-manager.hpp"
#include "renderer/vulkan/graphics/resource.hpp"

#include <print>
using std::println;

using namespace varicle::renderer;
using namespace varicle::renderer::vulkan;

varicle::renderer::vulkan::VulkanContext
varicle::renderer::vulkan::init(Window& window) {
    auto ctx = VulkanContext(window);

    println("VulkanRender: Initializing Vulkan");

    create_instance(ctx);
    if (enable_validation_layers)
        debug::setupDebugMessenger(ctx);

    create_surface(ctx);

    select_physical_device(ctx);
    create_logical_device(ctx);
    create_swap_chain(ctx);
    create_image_views(ctx);
    create_texture_sampler(ctx);

    create_command_pool(ctx);
    create_color_resources(ctx);
    create_depth_resources(ctx);

    create_command_buffers(ctx);
    create_sync_objects(ctx);

    return ctx;
}

void varicle::renderer::vulkan::set_clear_color(
    VulkanContext& ctx,
    Colorf         background
) {
    ctx.m_clear_color = vk::ClearColorValue(
        background.r, background.g, background.b, background.a
    );

    ctx.m_clear_depth = vk::ClearDepthStencilValue(1.0f, 0);
}

bool varicle::renderer::vulkan::should_close(VulkanContext& ctx) {
    return glfwWindowShouldClose(ctx.m_window.get_window());
}

void varicle::renderer::vulkan::begin_frame(VulkanContext& ctx) {

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
        ctx.m_color_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    // Transition swap chain image to optimal layout for rendering
    transition_image_layout(
        ctx,
        ctx.m_swap_chain_images[image_index],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    // Transition depth_image image to optimal depth layout
    transition_image_layout(
        ctx,
        ctx.m_depth_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
            vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
            vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::ImageAspectFlagBits::eDepth
    );

    vk::RenderingAttachmentInfo clear_attachment_info = {
        .imageView          = ctx.m_color_image_view,
        .imageLayout        = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode        = vk::ResolveModeFlagBits::eAverage,
        .resolveImageView   = ctx.m_swap_chain_image_views[image_index],
        .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp             = vk::AttachmentLoadOp::eClear,
        .storeOp            = vk::AttachmentStoreOp::eStore,
        .clearValue         = ctx.m_clear_color
    };

    vk::RenderingAttachmentInfo depth_attachment_Info = {
        .imageView   = ctx.m_depth_image_view,
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eDontCare,
        .clearValue  = ctx.m_clear_depth
    };

    vk::RenderingInfo rendering_info = {
        .renderArea = { .offset = { 0, 0 }, .extent = ctx.m_swap_chain_extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &clear_attachment_info,
        .pDepthAttachment     = &depth_attachment_Info
    };

    cmd.beginRendering(rendering_info);
    // cmd.bindPipeline(
    //     vk::PipelineBindPoint::eGraphics, default_pipeline_bundle->pipeline
    // );

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
}

void varicle::renderer::vulkan::end_frame(VulkanContext& ctx) {

    if (ctx.m_recreating_frame)
        return;
    ctx.m_command_buffers[ctx.m_frame_index].endRendering();
    const auto image_index = ctx.m_image_index;

    transition_image_layout(
        ctx,
        ctx.m_swap_chain_images[image_index],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor
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
        .pSignalSemaphores    = &ctx.m_render_finished_semaphores[image_index]
    };

    ctx.m_graphics_queue.submit(
        submitInfo, ctx.m_in_flight_fences[ctx.m_frame_index]
    );

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &ctx.m_render_finished_semaphores[image_index],
        .swapchainCount     = 1,
        .pSwapchains        = &ctx.m_swap_chain,
        .pImageIndices      = &image_index
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

void varicle::renderer::vulkan::draw_primitive(
    VulkanContext&     ctx,
    ResourceManager&   rm,
    PrimitiveType      type,
    MaterialHandle     material,
    const Transform3D& transform,
    RenderCamera&      camera
) {

    auto& mesh = rm.get_primitive(static_cast<uint32_t>(type));
    auto  cmd  = ctx.get_current_command_buffer();

    cmd.bindVertexBuffers(0, mesh.m_vertex_buffer, { 0 });

    cmd.bindIndexBuffer(
        mesh.m_index_buffer,
        0,
        vk::IndexTypeValue<decltype(mesh.m_indices)::value_type>::value
    );

    auto& mat = rm.get_material(material);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mat.m_graphics_pipeline);
    const auto mvp = varicle::get_model_matrix(transform);

    // Set camera asepect and update frame data
    std::tie(camera.camera.width, camera.camera.height) = ctx.m_window.size();
    camera.update(ctx.m_frame_index);

    cmd.pushConstants(
        mat.m_pipeline_layout,
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4),
        &mvp
    );

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        mat.m_pipeline_layout,
        0,
        { camera.descriptor_pool_alloc.descriptor_sets[ctx.m_frame_index],
          mat.m_descriptor_set },
        nullptr
    );

    cmd.drawIndexed(static_cast<uint32_t>(mesh.m_indices.size()), 1, 0, 0, 0);
}

void varicle::renderer::vulkan::cleanup(
    VulkanContext&    ctx,
    DescriptorSystem& ds,
    ResourceManager&  rm
) {

    ctx.m_device.waitIdle();

    std::println("Shutting down....");

    rm.unload_all_materials(ctx, ds);
    rm.unload_all_shaders(ctx.m_device);
    rm.unload_all_texures(ctx.m_device);
    rm.unload_all_meshes(ctx.m_device);

    ds.cache.reset(ctx.m_device);
    ds.pool_allocator.reset(ctx.m_device);

    // Synchronization
    for (auto& i : ctx.m_present_complete_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_render_finished_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_in_flight_fences)
        ctx.m_device.destroyFence(i);

    ctx.m_device.destroySampler(ctx.m_texture_sampler);

    ctx.m_device.destroyImageView(ctx.m_depth_image_view);
    ctx.m_device.freeMemory(ctx.m_depth_image_memory);
    ctx.m_device.destroyImage(ctx.m_depth_image);

    ctx.m_device.destroyImageView(ctx.m_color_image_view);
    ctx.m_device.freeMemory(ctx.m_color_image_memory);
    ctx.m_device.destroyImage(ctx.m_color_image);

    // Command pool with command buffers
    ctx.m_device.destroyCommandPool(ctx.m_command_pool);
    ctx.m_device.destroyCommandPool(ctx.m_transfer_command_pool);

    // Swap chain
    for (auto& view : ctx.m_swap_chain_image_views)
        ctx.m_device.destroyImageView(view);
    ctx.m_device.destroySwapchainKHR(ctx.m_swap_chain);

    // Device
    ctx.m_device.destroy();

    ctx.m_instance.destroySurfaceKHR(ctx.m_surface);
#if !defined(NDEBUG)
    ctx.m_instance.destroyDebugUtilsMessengerEXT(ctx.m_debugMessenger);
#endif
    ctx.m_instance.destroy();
}

ResourceManager
varicle::renderer::vulkan::create_resource_manager(VulkanContext& ctx) {
    ResourceManager rm;
    rm.load_primitives(ctx);
    return rm;
}

DescriptorSystem
varicle::renderer::vulkan::create_descriptor_system(VulkanContext& ctx) {
    return create_default_descriptor_system(ctx.m_device);
}

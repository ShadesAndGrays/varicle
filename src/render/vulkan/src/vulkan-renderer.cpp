#include "vulkan-renderer.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "core/device.hpp"
#include "core/impl.hpp"
#include "core/init.hpp"
#include "core/swap-chain.hpp"
#include "core/validation.hpp"
#include "graphics/buffer.hpp"
#include "graphics/image.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/resource.hpp"
#include "util/util.hpp"
#include <chrono>
#include <memory>
#include <print>

// const char* TEXTURE_PATH = "textures/cube.png";
using namespace varicle::render;
using namespace varicle::render::vulkan;

MeshHandle                      RECTANGLE;
TextureHandle                   DEFAULT_TEXTURE;
std::unique_ptr<PipelineBundle> default_pipeline_bundle;

void VulkanRenderer ::init(uint32_t width, uint32_t height, const char* title) {
    impl = new Impl{};

    std::println("VulkanRender: Initializing Vulkan");

    VulkanContext& ctx = impl->v_context;
    m_window.init(width, height, title);

    create_instance(ctx);
    if (enable_validation_layers)
        debug::setupDebugMessenger(ctx);
    create_surface(ctx, m_window);

    select_physical_device(ctx);
    create_logical_device(ctx);
    create_swap_chain(ctx);
    create_image_views(ctx);
    create_texture_sampler(ctx);
    // create_descriptor_set_layout(ctx);
    create_command_pool(ctx);
    create_color_resources(ctx);
    create_depth_resources(ctx);

    DEFAULT_TEXTURE = m_resource_manager.load_texture(ctx, "textures/cube.png");
    create_uniform_buffer(ctx);
    auto& texture = m_resource_manager.get_texture(ctx, DEFAULT_TEXTURE);

    auto shader_module =
        create_shader_module(ctx, readFile("shaders/slang.spv"));

    PipelineConfig pipeline_config{
        .color_format       = ctx.m_swap_chain_surface_format.format,
        .depth_format       = ctx.m_depth_format,
        .frag_shader_module = shader_module,
        .vert_shader_module = shader_module,
        .frag_entry_point   = "fragMain",
        .vert_entry_point   = "vertMain",
        // TODO: Use ranges instead of vectors
        .binding_descriptions   = Vertex::getBindingDescriptions(),
        .attribute_descriptions = Vertex::getAttributeDescriptions(),
        .msaa_samples           = vk::SampleCountFlagBits::e8,
        .push_constant_range    = sizeof(glm::mat4),

    };
    PipelineCreationSystem                      pc_system;
    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings = {
        vk::DescriptorSetLayoutBinding{
            0,
            vk::DescriptorType::eCombinedImageSampler,
            1,
            vk::ShaderStageFlagBits::eFragment }
    };
    std::vector<PipelineBundle> bundles;
    bundles.push_back(
        PipelineBundle(layout_bindings, { texture.m_descriptor_image_info }, {})
    );

    pc_system.build_descriptors(ctx.m_device, bundles);
    pc_system.create_graphics_pipeline(
        ctx.m_device, pipeline_config, bundles[0]
    );

    default_pipeline_bundle =
        std::make_unique<PipelineBundle>(std::move(bundles[0]));

    // // TODO: Add Bindless texture

    // create_descriptor_pool(ctx);
    // create_descriptor_set(
    //     ctx, m_resource_manager.get_texture(ctx, DEFAULT_TEXTURE)
    // );
    create_command_buffers(ctx);
    create_sync_objects(ctx);

    // Mesh rectangle(
    //     { { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
    //       { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    //       { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    //       { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } } },
    //     { 0, 1, 2, 2, 3, 0 }
    // );
    //
    // create_vertex_buffer(ctx, rectangle);
    // create_index_buffer(ctx, rectangle);
    // RECTANGLE = m_resource_manager.add_mesh(ctx, std::move(rectangle));
    // std::println("mesh handle {}",Rectangle);
}

// bool VulkanRenderer::should_close_window() {
//     return glfwWindowShouldClose(impl->v_context.m_window);
// }

void VulkanRenderer::resize(uint32_t width, uint32_t height) {
    auto& ctx  = impl->v_context;
    ctx.width  = width;
    ctx.height = height;
    glfwSetWindowSize(m_window.get_window(), width, height);
}

bool varicle::render::vulkan::VulkanRenderer::should_close_window() {
    return glfwWindowShouldClose(m_window.get_window());
}

void VulkanRenderer::shutdown() {

    auto& ctx = impl->v_context;
    // pause tile gpu isn't doing anything
    ctx.m_device.waitIdle();

    std::println("Shutting down....");

    // Synchronization
    for (auto& i : ctx.m_present_complete_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_render_finished_semaphores)
        ctx.m_device.destroySemaphore(i);
    for (auto& i : ctx.m_in_flight_fences)
        ctx.m_device.destroyFence(i);

    // Shader
    ctx.m_device.destroyShaderModule(ctx.m_shader_module);

    ctx.m_device.destroyDescriptorPool(ctx.m_descriptor_pool);

    m_resource_manager.unload_all_meshes(ctx);
    m_resource_manager.unload_all_texures(ctx);
    ctx.m_device.destroySampler(ctx.m_texture_sampler);
    // images
    // ctx.m_device.destroyImageView(ctx.m_texture_image_view);
    // ctx.m_device.freeMemory(ctx.m_texture_image_memory);
    // ctx.m_device.destroyImage(ctx.m_texture_image);

    ctx.m_device.destroyImageView(ctx.m_depth_image_view);
    ctx.m_device.freeMemory(ctx.m_depth_image_memory);
    ctx.m_device.destroyImage(ctx.m_depth_image);

    ctx.m_device.destroyImageView(ctx.m_color_image_view);
    ctx.m_device.freeMemory(ctx.m_color_image_memory);
    ctx.m_device.destroyImage(ctx.m_color_image);

    // Buffers
    // ctx.m_device.freeMemory(ctx.m_vertex_buffer_memory);
    // ctx.m_device.freeMemory(ctx.m_index_buffer_memory);
    // ctx.m_device.destroyBuffer(ctx.m_vertex_buffer);
    // ctx.m_device.destroyBuffer(ctx.m_index_buffer);

    for (auto& i : ctx.m_uniform_buffers_memory) {
        ctx.m_device.freeMemory(i);
    }
    for (auto& i : ctx.m_uniform_buffers) {
        ctx.m_device.destroyBuffer(i);
    }

    // Graphics pipeline
    ctx.m_device.destroyPipeline(ctx.m_graphics_pipeline);
    ctx.m_device.destroyPipelineLayout(ctx.m_pipeline_layout);
    ctx.m_device.destroyDescriptorSetLayout(ctx.m_descriptor_set_layout);

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

    // call last
    if (impl != nullptr)
        delete impl;
}

void VulkanRenderer::set_clear_color(Color background) {
    auto& ctx         = impl->v_context;
    ctx.m_clear_color = vk::ClearColorValue(
        background.r, background.g, background.b, background.a
    );

    ctx.m_clear_depth = vk::ClearDepthStencilValue(1.0f, 0);
}

void VulkanRenderer::begin_frame(bool clear_screen) {
    auto& ctx              = impl->v_context;
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
        .loadOp             = clear_screen ? vk::AttachmentLoadOp::eClear
                                           : vk::AttachmentLoadOp::eLoad,
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
    cmd.bindPipeline(
        vk::PipelineBindPoint::eGraphics, default_pipeline_bundle->pipeline
    );

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

void VulkanRenderer::end_frame() {

    auto& ctx = impl->v_context;
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
TextureHandle VulkanRenderer::load_texture(const char* filepath) {
    auto& ctx = impl->v_context;
    return m_resource_manager.load_texture(ctx, filepath);
}

void VulkanRenderer::destroy_texture(TextureHandle texture) {
    auto& ctx = impl->v_context;
    m_resource_manager.unload_texture(ctx, texture);
}

TextureHandle VulkanRenderer::load_mesh(const char* filepath) {

    auto& ctx = impl->v_context;
    return m_resource_manager.load_mesh(ctx, filepath);
};

void VulkanRenderer::destroy_mesh(MeshHandle mesh) {

    auto& ctx = impl->v_context;
    m_resource_manager.unload_mesh(ctx, mesh);
};

void VulkanRenderer::draw_rect(const Rect& rect, const Color& color) {
    auto&             ctx = impl->v_context;
    vk::CommandBuffer cmd = ctx.m_command_buffers[ctx.m_frame_index];

    const std::vector<Vertex> vertices{
        { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4
    };

}

void VulkanRenderer::draw_object(Object object) {

    auto& ctx = impl->v_context;

    auto& mesh = m_resource_manager.get_mesh(ctx, object.mesh);
    auto  cmd  = ctx.get_current_command_buffer();

    static auto start_time   = std::chrono::high_resolution_clock::now();
    auto        current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     current_time - start_time
    )
                     .count();

    glm::mat4 mvp = m_camera.get_projection_matix() *
        m_camera.get_view_matrix() * object.get_model_matrix();

    cmd.pushConstants(
        default_pipeline_bundle->pipeline_layout,
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4),
        &mvp
    );
    // );

    cmd.bindVertexBuffers(0, mesh.m_vertex_buffer, { 0 });

    cmd.bindIndexBuffer(
        mesh.m_index_buffer,
        0,
        vk::IndexTypeValue<decltype(mesh.m_indices)::value_type>::value
    );

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        default_pipeline_bundle->pipeline_layout,
        0,
        default_pipeline_bundle->descriptor_sets[ctx.m_frame_index],
        nullptr
    );

    cmd.drawIndexed(static_cast<uint32_t>(mesh.m_indices.size()), 1, 0, 0, 0);
}

void VulkanRenderer::set_camera(Camera camera) {
    camera = camera;
};
Camera& VulkanRenderer::get_camera() {
    return m_camera;
};

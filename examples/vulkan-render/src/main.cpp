#include <varicle.hpp>

#include "renderer/common/camera.hpp"
#include "renderer/vulkan/graphics/render-camera.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include "renderer/vulkan/util/slang-compiler.hpp"
#include <print>
using std::println;

namespace v = varicle::renderer::vulkan;

int main() {
    varicle::renderer::Window window(
        400, 400, "Vulkan Renderer", varicle::renderer::GraphicsBackend::Vulkan
    );

    auto ctx = v::init(window);
    auto rm  = v::create_resource_manager(ctx);
    auto ds  = v::create_descriptor_system(ctx);

    // Create Texture
    const auto DEFAULT_TEXTURE =
        rm.add_texture(std::move(rm.create_default_texture(ctx)));

    // Get Camera data
    auto render_camera = v::RenderCamera(ctx, ds);

    // Shader uniform Bindings

    // Both variables are the first in thir own set
    // We've sepearted them cause of how we will update
    // Textures are only updated once while the frame data needs to be updated
    // evrey frame
    // If they were together we would need to update the textrue every frames as
    // well

    auto texture_layout_set_binding = vk::DescriptorSetLayoutBinding{
        0,
        vk::DescriptorType::eCombinedImageSampler,
        1,
        vk::ShaderStageFlagBits::eFragment
    };

    auto texture_layout_set = ds.cache.get_or_create_layout(
        ctx.m_device,
        {
            texture_layout_set_binding,
        }
    );

    // He we reuse the layout set. To give use two identitcal frame data set
    // along with our texture layout set
    auto set = ds.pool_allocator.allocate(ctx.m_device, { texture_layout_set });

    // Finally we write to our descriptor set so our shader can see the data
    // we've created

    ds.writer.write_texture(
        ctx.m_device,
        texture_layout_set_binding,
        set.descriptor_sets[0],
        std::array{ rm.get_texture(DEFAULT_TEXTURE).m_descriptor_image_info }
    );

    // Now it's time to build our shader

    // I've made a nice bundle container to keep everything in order
    // The order of the layout should match the order in the shader
    v::PipelineBundle default_bundle(
        { render_camera.frame_data_layout_set, texture_layout_set }
    );

    auto shader_handles = rm.load_shader(
        ctx,
        "assets/vulkan-shaders/shader.slang",
        { "vertMain", "fragMain" },
        { "assets/vulkan-shaders/include" }
    );

    auto vert = shader_handles[0];
    auto frag = shader_handles[1];

    // Configuer pipeline stages
    v::PipelineConfig config{
        .color_format           = ctx.m_swap_chain_surface_format.format,
        .depth_format           = ctx.m_depth_format,
        .frag_shader_module     = rm.get_shader(frag),
        .vert_shader_module     = rm.get_shader(vert),
        .binding_descriptions   = v::getBindingDescriptions(),
        .attribute_descriptions = v::getAttributeDescriptions(),
        .cull_mode              = vk::CullModeFlagBits::eNone,
        .front_face             = vk::FrontFace::eClockwise,
        .msaa_samples           = ctx.m_msaa_samples,
        .push_constant_range    = sizeof(glm::mat4),
    };

    // Pipelin is goes a head an poplulates bundle with the the pipeline and
    // it's layout
    v::create_graphics_pipeline(ctx.m_device, config, default_bundle);

    // material has one texture slot
    v::MaterialData mat0{ texture_layout_set,
                          default_bundle.pipeline_layout,
                          default_bundle.pipeline,
                          set.descriptor_sets[0],
                          set.descriptor_pool_idx,
                          vert,
                          frag };

    auto m_handle = rm.add_material(ctx, std::move(mat0));

    v::set_clear_color(ctx, { 0.7, 0.8, 0.9, 1.0 });

    float value = 0.0f;
    while (!v::should_close(ctx)) {
        // value += 0.001667;
        //

        v::begin_frame(ctx);
        // We draw our triangle
        v::draw_primitive(
            ctx,
            rm,
            varicle::renderer::PrimitiveType::QUAD,
            m_handle,
            {
                { 0, 0, 0 }, //  sin(value) * 200 },
                { 1, 1, 1 },
                { 0, value, 0 },
            },
            render_camera
        );

        // value += 0.013667;
        v::end_frame(ctx);
    }

    ctx.m_device.waitIdle();
    render_camera.destory(ctx.m_device, ds);

    v::destroy_graphics_pipeline(ctx.m_device, default_bundle);
    v::cleanup(ctx, ds, rm);

    return 0;
}

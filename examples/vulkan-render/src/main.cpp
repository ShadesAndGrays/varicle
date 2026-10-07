#include <varicle.hpp>

#include "renderer/vulkan/util/slang-compiler.hpp"

namespace v = varicle::renderer::vulkan;

int main() {
    varicle::renderer::Window window(
        800, 600, "Vulkan Renderer", varicle::renderer::GraphicsBackend::Vulkan
    );

    auto ctx = v::init(window);
    auto rm  = v::create_resource_manager(ctx);
    auto ds  = v::create_descriptor_system(ctx);
    // ds.pool_allocator.create_new_pool(ctx.m_device);

    const auto DEFAULT_TEXTURE =
        rm.add_texture(std::move(rm.create_default_texture(ctx)));

    // Create default layout
    ds.pool_allocator.create_new_pool(ctx.m_device);
    auto default_layout_set_binding = vk::DescriptorSetLayoutBinding{
        0,
        vk::DescriptorType::eCombinedImageSampler,
        1,
        vk::ShaderStageFlagBits::eFragment
    };

    auto default_layout_set = ds.cache.get_or_create_layout(
        ctx.m_device, { default_layout_set_binding }
    );

    auto sets    = ds.pool_allocator.allocate(ctx.m_device, default_layout_set);
    auto texture = std::vector<vk::DescriptorImageInfo>{
        rm.get_texture(DEFAULT_TEXTURE).m_descriptor_image_info
    };

    ds.writer.write_texture(
        ctx.m_device, default_layout_set_binding, sets[0], texture
    );
    ds.writer.write_texture(
        ctx.m_device, default_layout_set_binding, sets[1], texture
    );

    v::PipelineBundle default_bundle({ default_layout_set });

    auto compiled_shaders = compile_shader_code(
        "assets/vulkan-shaders/shader.slang", { "vertMain", "fragMain" }
    );

    // auto shader_s      = v::read_file("assets/vulkan-shaders/slang.spv");
    auto shader_v      = compiled_shaders[0].code;
    auto shader_v_module = v::create_shader_module(ctx, shader_v);

    auto shader_f      = compiled_shaders[1].code;
    auto shader_f_module = v::create_shader_module(ctx, shader_f);

    v::PipelineConfig config{
        .color_format           = ctx.m_swap_chain_surface_format.format,
        .depth_format           = ctx.m_depth_format,
        .frag_shader_module     = shader_f_module,
        .vert_shader_module     = shader_v_module,
        .binding_descriptions   = v::getBindingDescriptions(),
        .attribute_descriptions = v::getAttributeDescriptions(),
        .msaa_samples           = vk::SampleCountFlagBits::e8,
        .push_constant_range    = sizeof(glm::mat4),
    };

    v::create_graphics_pipeline(ctx.m_device, config, default_bundle);

    v::MaterialData mat0{ default_layout_set,
                          default_bundle.pipeline_layout,
                          default_bundle.pipeline,
                          std::vector<vk::DescriptorSet>{ sets.begin(),
                                                          sets.end() } };

    rm.add_material(ctx, std::move(mat0));

    v::set_clear_color(ctx, { 0.7, 0.8, 0.9, 1.0 });

    while (!v::should_close(ctx)) {
        v::begin_frame(ctx);
        v::draw_primitive(ctx, rm, varicle::renderer::PrimitiveType::TRIANGLE);
        v::end_frame(ctx);
    }

    v::cleanup(ctx);

    return 0;
}

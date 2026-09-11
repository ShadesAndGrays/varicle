#include "graphics/pipeline.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "graphics/resource.hpp"
#include "util/util.hpp"

namespace varicle::render::vulkan {

void create_descriptor_set(VulkanContext& ctx, std::span<Texture> textures) {
    std::vector<vk::DescriptorSetLayout> layouts(
        MAX_FRAMES_IN_FLIGHT, ctx.m_descriptor_set_layout
    );
    vk::DescriptorSetAllocateInfo alloc_info{
        .descriptorPool     = ctx.m_descriptor_pool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts        = layouts.data()
    };

    // This guy here is where we allocate the memory
    ctx.m_descriptor_sets = ctx.m_device.allocateDescriptorSets(alloc_info);

    // std::println("Descriptor set count: {}", ctx.m_descriptor_sets.size());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo buffer_info{

            .buffer = ctx.m_uniform_buffers[i],
            .offset = 0,
            .range  = sizeof(UniformBufferObject)
        };

        // Here we create and array of image views and allocate all of them at
        // once
        std::vector<vk::DescriptorImageInfo> image_infos;

        image_infos.reserve(textures.size());
        for (auto i = 0; i < textures.size(); i++) {
            image_infos[i] = vk::DescriptorImageInfo{
                .sampler     = ctx.m_texture_sampler,
                .imageView   = textures[i].m_image_view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
            };
        }

        std::array<vk::WriteDescriptorSet, 2> descriptor_write

            { { { .dstSet          = ctx.m_descriptor_sets[i],
                  .dstBinding      = 0,
                  .dstArrayElement = 0,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eUniformBuffer,
                  .pBufferInfo     = &buffer_info },

                // This was for a single image
                // { .dstSet          = ctx.m_descriptor_sets[i],
                //   .dstBinding      = 1,
                //   .dstArrayElement = 0,
                //   .descriptorCount = 1,
                //   .descriptorType =
                //       vk::DescriptorType::eCombinedImageSampler,
                //   .pImageInfo = &image_info }

                // this is now for our array
                { .dstSet          = ctx.m_descriptor_sets[i],
                  .dstBinding      = 1,
                  .dstArrayElement = 0,
                  .descriptorCount = static_cast<uint32_t>(image_infos.size()),
                  .descriptorType  = vk::DescriptorType::eSampledImage,
                  .pImageInfo      = image_infos.data() }

            } };

        ctx.m_device.updateDescriptorSets(descriptor_write, {});
    }
}

void create_descriptor_set_layout(VulkanContext& ctx) {
    /*
     * Creating a layout for binding
     */
    std::array<vk::DescriptorSetLayoutBinding, 2> ubo_layout_bindings{
        { { .binding         = 0,
            .descriptorType  = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags      = vk::ShaderStageFlagBits::eVertex },
          { .binding         = 1,
            .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags      = vk::ShaderStageFlagBits::eFragment } }
    };
    vk::DescriptorSetLayoutCreateInfo layout_info{
        .bindingCount = static_cast<uint32_t>(ubo_layout_bindings.size()),
        .pBindings    = ubo_layout_bindings.data()

    };
    ctx.m_descriptor_set_layout =
        ctx.m_device.createDescriptorSetLayout(layout_info);
}

void create_descriptor_pool(VulkanContext& ctx) {

    std::array<vk::DescriptorPoolSize, 2> pool_size{
        { {
              .type            = vk::DescriptorType::eUniformBuffer,
              .descriptorCount = MAX_FRAMES_IN_FLIGHT,
          },
          {
              .type            = vk::DescriptorType::eCombinedImageSampler,
              .descriptorCount = MAX_FRAMES_IN_FLIGHT,

          } }
    };

    /*
     * eFreeDescriptorSet allows use to free the descriptor
     * Inadequate descriptor pools are problems that validation layers man not
     * catch. Some may throw OutOfPoolMemory others the GPU may resolve it's
     * self
     */

    vk::DescriptorPoolCreateInfo pool_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(pool_size.size()),
        .pPoolSizes    = pool_size.data()
    };

    ctx.m_descriptor_pool = ctx.m_device.createDescriptorPool(pool_info);
}

vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code) {

    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode    = reinterpret_cast<const uint32_t*>(code.data())
    };
    vk::ShaderModule shaderModule(ctx.m_device.createShaderModule(createInfo));
    return shaderModule;
}

/* Programmable Function Stages
 * Fixed Function Stages
 */
void create_graphics_pipeline(VulkanContext& ctx) {

    ctx.m_shader_module =
        create_shader_module(ctx, readFile("shaders/slang.spv"));
    vk::ShaderModule& shaderModule = ctx.m_shader_module;

    vk::PipelineShaderStageCreateInfo vert_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule,
        .pName  = "vertMain"

    };

    vk::PipelineShaderStageCreateInfo frag_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModule,
        .pName  = "fragMain"
    };

    vk::PipelineShaderStageCreateInfo shader_stages[] = {
        vert_shader_stageInfo, frag_shader_stageInfo
    };

    auto binding_description    = Vertex::getBindingDescription();
    auto attribute_descriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertex_inputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions    = &binding_description,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions = attribute_descriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo input_assembly{
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    // vk::Viewport viewport{ 0.0f,
    //                        0.0f,
    //                        static_cast<float>(ctx.m_swap_chain_extent.width),
    //                        static_cast<float>(ctx.m_swap_chain_extent.height),
    //                        0.0f,
    //                        1.0f };
    //
    // vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, ctx.m_swap_chain_extent };

    std::vector<vk::DynamicState> dynamic_states = {
        vk::DynamicState::eViewport, vk::DynamicState::eScissor
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    vk::PipelineViewportStateCreateInfo viewport_state{ .viewportCount = 1,
                                                        .scissorCount  = 1 };

    vk::PipelineDepthStencilStateCreateInfo depth_stencil{
        .depthTestEnable       = vk::True,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False,
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = vk::PolygonMode::eFill,
        .cullMode                = vk::CullModeFlagBits::eNone,
        // .frontFace               = vk::FrontFace::eClockwise,
        .frontFace       = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth       = 1.0f
    };

    // Anti-aliasing. Not enabled for now
    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = ctx.m_msaa_samples,
        .sampleShadingEnable  = vk::True,
        .minSampleShading     = 0.2f

    };

    vk::PipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = vk::False,

        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp        = vk::BlendOp::eAdd,

        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp        = vk::BlendOp::eAdd,

        .colorWriteMask = vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo color_blending{
        .logicOpEnable   = vk::False,
        .logicOp         = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment
    };

    vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset     = 0,
        .size       = sizeof(glm::mat4)

    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount         = 1,
        .pSetLayouts            = &ctx.m_descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range
    };

    ctx.m_pipeline_layout = vk::PipelineLayout(
        ctx.m_device.createPipelineLayout(pipelineLayoutInfo)
    );

    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{
        .stageCount          = 2,
        .pStages             = shader_stages,
        .pVertexInputState   = &vertex_inputInfo,
        .pInputAssemblyState = &input_assembly,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depth_stencil,
        .pColorBlendState    = &color_blending,
        .pDynamicState       = &dynamic_state,
        .layout              = ctx.m_pipeline_layout,
        .renderPass          = nullptr
    };

    vk::PipelineRenderingCreateInfo pipeline_rendering_create_info{
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &ctx.m_swap_chain_surface_format.format,
        .depthAttachmentFormat   = ctx.m_depth_format
    };

    vk::StructureChain<
        vk::GraphicsPipelineCreateInfo,
        vk::PipelineRenderingCreateInfo>
        pipelineCreateInfoChain = { graphics_pipeline_create_info,
                                    pipeline_rendering_create_info };

    auto result = ctx.m_device.createGraphicsPipeline(
        nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    );

    ctx.m_graphics_pipeline = result.value;
}
} // namespace varicle::render::vulkan

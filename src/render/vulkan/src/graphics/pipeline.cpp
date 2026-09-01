#include "graphics/pipeline.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "core/vertex.hpp"
#include "util/util.hpp"
#include <print>

namespace varicle::render::vulkan {

void create_descriptor_set(VulkanContext& ctx) {
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

    std::println("Descriptor set count: {}", ctx.m_descriptor_sets.size());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo buffer_info{

            .buffer = ctx.m_uniform_buffers[i],
            .offset = 0,
            .range  = sizeof(UniformBufferObject)
        };
        vk::DescriptorImageInfo image_info{
            .sampler     = ctx.m_texture_sampler,
            .imageView   = ctx.m_texture_image_view,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };

        std::array<vk::WriteDescriptorSet, 2> descriptor_write

            { { { .dstSet          = ctx.m_descriptor_sets[i],
                  .dstBinding      = 0,
                  .dstArrayElement = 0,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eUniformBuffer,
                  .pBufferInfo     = &buffer_info },

                { .dstSet          = ctx.m_descriptor_sets[i],
                  .dstBinding      = 1,
                  .dstArrayElement = 0,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                  .pImageInfo     = &image_info } } };

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

void create_graphics_pipeline(VulkanContext& ctx) {

    ctx.m_shader_module =
        create_shader_module(ctx, readFile("shaders/slang.spv"));
    vk::ShaderModule& shaderModule = ctx.m_shader_module;

    // NOTE: What is the name of the types of pipeline?
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule,
        .pName  = "vertMain"

    };

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModule,
        .pName  = "fragMain"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
                                                         fragShaderStageInfo };

    auto bindingDescription    = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions    = &bindingDescription,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::Viewport viewport{ 0.0f,
                           0.0f,
                           static_cast<float>(ctx.m_swap_chain_extent.width),
                           static_cast<float>(ctx.m_swap_chain_extent.height),
                           0.0f,
                           1.0f };

    vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, ctx.m_swap_chain_extent };

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport, vk::DynamicState::eScissor
    };
    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates    = dynamicStates.data()
    };

    vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1,
                                                       .scissorCount  = 1 };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = vk::PolygonMode::eFill,
        .cullMode                = vk::CullModeFlagBits::eBack,
        // .frontFace               = vk::FrontFace::eClockwise,
        .frontFace       = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth       = 1.0f
    };

    // Anti-aliasing. Not enabled for now
    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable  = vk::False
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
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

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable   = vk::False,
        .logicOp         = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttachment
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount         = 1,
        .pSetLayouts            = &ctx.m_descriptor_set_layout,
        .pushConstantRangeCount = 0
    };

    ctx.m_pipeline_layout = vk::PipelineLayout(
        ctx.m_device.createPipelineLayout(pipelineLayoutInfo)
    );

    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &ctx.m_swap_chain_surface_format.format
    };

    vk::StructureChain<
        vk::GraphicsPipelineCreateInfo,
        vk::PipelineRenderingCreateInfo>
        pipelineCreateInfoChain =

            { { .stageCount          = 2,
                .pStages             = shaderStages,
                .pVertexInputState   = &vertexInputInfo,
                .pInputAssemblyState = &inputAssembly,
                .pViewportState      = &viewportState,
                .pRasterizationState = &rasterizer,
                .pMultisampleState   = &multisampling,
                .pColorBlendState    = &colorBlending,
                .pDynamicState       = &dynamicState,
                .layout              = ctx.m_pipeline_layout,
                .renderPass          = nullptr },
              { .colorAttachmentCount = 1,
                .pColorAttachmentFormats =
                    &ctx.m_swap_chain_surface_format.format } };

    auto result = ctx.m_device.createGraphicsPipeline(
        nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    );

    ctx.m_graphics_pipeline = result.value;
}
} // namespace varicle::render::vulkan

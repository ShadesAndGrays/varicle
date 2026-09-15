
#include "graphics/pipeline.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "graphics/resource.hpp"
#include "util/util.hpp"

namespace varicle::render::vulkan {

vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code) {

    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode    = reinterpret_cast<const uint32_t*>(code.data())
    };
    vk::ShaderModule shaderModule(ctx.m_device.createShaderModule(createInfo));
    return shaderModule;
}

void PipelineCreationSystem::build_descriptors(
    vk::Device                   device,
    std::vector<PipelineBundle>& bundles
) {

    // Reset trackers
    m_descriptor_set_count = 0;
    m_combined_layouts.clear();

    // Creates all set layouts and track layout counts
    for (auto& bundle : bundles) {
        bundle.descriptor_set_layout = create_descriptor_set_layout(
            device, bundle.descriptor_set_layout_bindings
        );
    }

    auto pool = calculate_descriptor_pool(device);

    for (auto& bundle : bundles) {
        bundle.descriptor_sets =
            create_descriptor_set(device, bundle.descriptor_set_layout, pool);
        apply_writes(device, create_descriptor_writes(bundle));
    }
}

vk::DescriptorSetLayout PipelineCreationSystem::create_descriptor_set_layout(
    vk::Device                                         device,
    const std::vector<vk::DescriptorSetLayoutBinding>& layout_binding
) {

    for (auto& i : layout_binding) {
        if (m_combined_layouts.contains(i.descriptorType)) {
            m_combined_layouts[i.descriptorType] += i.descriptorCount;
        } else {
            m_combined_layouts[i.descriptorType] = i.descriptorCount;
        }
    }

    m_descriptor_set_count += 1;

    vk::DescriptorSetLayoutCreateInfo layout_info{
        .bindingCount = static_cast<uint32_t>(layout_binding.size()),
        .pBindings    = layout_binding.data()
    };

    return device.createDescriptorSetLayout(layout_info);
}

vk::DescriptorPool
PipelineCreationSystem::calculate_descriptor_pool(vk::Device device) {

    std::vector<vk::DescriptorPoolSize> pool_size{};
    pool_size.reserve(m_combined_layouts.size());

    uint32_t pool_size_idx = 0;
    for (auto combined_layout : m_combined_layouts) {
        pool_size.push_back(
            { .type            = combined_layout.first,
              .descriptorCount = combined_layout.second * MAX_FRAMES_IN_FLIGHT }
        );
    }

    vk::DescriptorPoolCreateInfo pool_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = MAX_FRAMES_IN_FLIGHT * m_descriptor_set_count,
        .poolSizeCount = static_cast<uint32_t>(pool_size.size()),
        .pPoolSizes    = pool_size.data()
    };

    return device.createDescriptorPool(pool_info);
}

std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT>
PipelineCreationSystem::create_descriptor_set(
    vk::Device              device,
    vk::DescriptorSetLayout layout,
    vk::DescriptorPool      pool
) {

    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);

    vk::DescriptorSetAllocateInfo alloc_info{
        .descriptorPool     = pool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts        = layouts.data()
    };

    // This guy here is where we allocate the memory
    auto v_sets = device.allocateDescriptorSets(alloc_info);
    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> sets;
    std::copy(
        v_sets.begin(), v_sets.begin() + MAX_FRAMES_IN_FLIGHT, sets.begin()
    );

    return sets;
}

std::vector<vk::WriteDescriptorSet>
PipelineCreationSystem::create_descriptor_writes(PipelineBundle& bundle) {
    std::vector<vk::WriteDescriptorSet> writes;

    for (auto& layout_binding : bundle.descriptor_set_layout_bindings) {

        if (layout_binding.descriptorType ==
            vk::DescriptorType::eCombinedImageSampler) {
            for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                writes.push_back(
                    { .dstSet          = bundle.descriptor_sets[i],
                      .dstBinding      = layout_binding.binding,
                      .dstArrayElement = 0,
                      .descriptorCount = layout_binding.descriptorCount,
                      .descriptorType  = layout_binding.descriptorType,
                      .pImageInfo      = bundle.descriptor_images.data() }
                );
        } else if (
            layout_binding.descriptorType == vk::DescriptorType::eUniformBuffer
        ) {
            for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                writes.push_back(
                    { .dstSet          = bundle.descriptor_sets[i],
                      .dstBinding      = layout_binding.binding,
                      .dstArrayElement = 0,
                      .descriptorCount = layout_binding.descriptorCount,
                      .descriptorType  = layout_binding.descriptorType,
                      .pBufferInfo     = bundle.descriptor_buffers.data() }
                );
        } else {
            throw std::runtime_error(
                "Failed to craete descriptor writes. Descriptor type not "
                "supported "
            );
        }
    }

    return writes;
}
void PipelineCreationSystem::apply_writes(
    vk::Device                                 device,
    const std::vector<vk::WriteDescriptorSet>& writes
) {
    device.updateDescriptorSets(writes, {});
}

void PipelineCreationSystem::create_graphics_pipeline(
    vk::Device      device,
    PipelineConfig  config,
    PipelineBundle& bundle
) {

    vk::PipelineShaderStageCreateInfo vert_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = config.vert_shader_module,
        .pName  = config.vert_entry_point

    };

    vk::PipelineShaderStageCreateInfo frag_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = config.frag_shader_module,
        .pName  = config.frag_entry_point
    };

    vk::PipelineShaderStageCreateInfo shader_stages[] = {
        vert_shader_stageInfo, frag_shader_stageInfo
    };

    vk::PipelineVertexInputStateCreateInfo vertex_inputInfo{
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(config.binding_descriptions.size()),

        .pVertexBindingDescriptions = config.binding_descriptions.data(),

        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(config.attribute_descriptions.size()),

        .pVertexAttributeDescriptions = config.attribute_descriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo input_assembly{
        .topology = config.topology
    };

    // TODO: We will just handle this manual for now
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
        .polygonMode             = config.polygon_mode,
        .cullMode                = config.cull_mode,
        .frontFace               = config.front_face,
        .depthBiasEnable         = vk::False,
        .lineWidth               = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = config.msaa_samples,
        .sampleShadingEnable  = config.sample_enable,
        .minSampleShading     = 0.2f
    };

    vk::PipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = config.blending_enabled,

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
        .logicOpEnable   = config.blending_logic_enabled,
        .logicOp         = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment
    };

    vk::PushConstantRange push_constant_range{
        .stageFlags = config.push_constant_stage,
        .offset     = 0,
        .size       = config.push_constant_range

    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount         = 1,
        .pSetLayouts            = &bundle.descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range
    };

   bundle.pipeline_layout =
        device.createPipelineLayout(pipelineLayoutInfo);

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
        .layout              = bundle.pipeline_layout,
        .renderPass          = nullptr
    };

    vk::PipelineRenderingCreateInfo pipeline_rendering_create_info{
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &config.color_format,
        .depthAttachmentFormat   = config.depth_format
    };

    vk::StructureChain<
        vk::GraphicsPipelineCreateInfo,
        vk::PipelineRenderingCreateInfo>
        pipelineCreateInfoChain = { graphics_pipeline_create_info,
                                    pipeline_rendering_create_info };

    auto result = device.createGraphicsPipeline(
        nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    );

    bundle.pipeline        = result.value;
}

} // namespace varicle::render::vulkan

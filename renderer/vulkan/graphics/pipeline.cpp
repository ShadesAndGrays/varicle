
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/core/context.hpp"
#include <exception>
#include <print>
#include <tuple>

using std::println;

namespace varicle::renderer::vulkan {

vk::ShaderModule create_shader_module(
    const VulkanContext&         ctx,
    const std::vector<uint32_t>& code
) {

    vk::ShaderModuleCreateInfo createInfo{ .codeSize =
                                               code.size() * sizeof(uint32_t),
                                           .pCode = code.data() };
    vk::ShaderModule shaderModule(ctx.m_device.createShaderModule(createInfo));
    return shaderModule;
}

void create_graphics_pipeline(
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
        .depthTestEnable  = vk::True, // set to true after getting object to how
        .depthWriteEnable = vk::True,
        .depthCompareOp   = vk::CompareOp::eLess, // chang change back to less
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
        .setLayoutCount =
            static_cast<uint32_t>(bundle.descriptor_set_layout.size()),
        .pSetLayouts            = bundle.descriptor_set_layout.data(),
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range
    };

    bundle.pipeline_layout = device.createPipelineLayout(pipelineLayoutInfo);

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

    bundle.pipeline = result.value;
}

void destroy_graphics_pipeline(vk::Device device, PipelineBundle& bundle) {
    device.destroyPipeline(bundle.pipeline);
    device.destroyPipelineLayout(bundle.pipeline_layout);
}

DescriptorSystem create_default_descriptor_system(vk::Device device) {
    DescriptorLayoutCache   lc{};
    DescriptorPoolAllocator dpa{};
    DescriptorWriter        writer{};
    return { std::move(lc), std::move(dpa), std::move(writer) };
}

void DescriptorWriter::write_buffer(
    vk::Device                                device,
    vk::DescriptorSetLayoutBinding            layout_binding,
    vk::DescriptorSet                         descriptor_set,
    std::span<const vk::DescriptorBufferInfo> buffer_infos,
    std::uint32_t                             offset

) {

    std::vector<vk::WriteDescriptorSet> writes;
    writes.push_back(
        { .dstSet          = descriptor_set,
          .dstBinding      = layout_binding.binding,
          .dstArrayElement = offset,
          .descriptorCount = static_cast<uint32_t>(buffer_infos.size()),
          .descriptorType  = layout_binding.descriptorType,
          .pBufferInfo     = buffer_infos.data() }
    );

    device.updateDescriptorSets(writes, {});
}
void DescriptorWriter::write_texture(
    vk::Device                               device,
    vk::DescriptorSetLayoutBinding           layout_binding,
    vk::DescriptorSet                        descriptor_set,
    std::span<const vk::DescriptorImageInfo> image_infos,
    std::uint32_t                            offset
) {
    std::vector<vk::WriteDescriptorSet> writes;

    writes.push_back(
        { .dstSet          = descriptor_set,
          .dstBinding      = layout_binding.binding,
          .dstArrayElement = offset,
          .descriptorCount = static_cast<uint32_t>(image_infos.size()),
          .descriptorType  = layout_binding.descriptorType,
          .pImageInfo      = image_infos.data() }
    );

    device.updateDescriptorSets(writes, {});
}
void DescriptorPoolAllocator::create_new_pool(vk::Device device) {
    std::array<vk::DescriptorPoolSize, 2> pool_size{
        vk::DescriptorPoolSize{
            vk::DescriptorType::eCombinedImageSampler,
            s_samplers,
        },
        vk::DescriptorPoolSize{
            vk::DescriptorType::eUniformBuffer,
            s_samplers,
        }
    };

    vk::DescriptorPoolCreateInfo pool_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = s_maxSets,
        .poolSizeCount = static_cast<uint32_t>(pool_size.size()),
        .pPoolSizes    = pool_size.data()
    };

    // if (m_current != nullptr) {
    //     m_full.push_back(m_current);
    // }
    auto pool = device.createDescriptorPool(pool_info);
    m_pool_stack.push_back(pool);
}

DescriptorPoolAllocator::Allocation DescriptorPoolAllocator::allocate(
    vk::Device                           device,
    std::vector<vk::DescriptorSetLayout> layouts
) {
    if (layouts.empty())
        return {};

    if (m_pool_stack.empty()) {
        create_new_pool(device);
    }

    vk::DescriptorSetAllocateInfo alloc_info{
        .descriptorPool     = m_pool_stack.back(),
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts        = layouts.data()
    };

    try {
        return { device.allocateDescriptorSets(alloc_info),
                 static_cast<uint32_t>(m_pool_stack.size() - 1) };

    } catch (vk::OutOfPoolMemoryError e) {
        // create new pool pushes old pool out
        create_new_pool(device);

        // update pool pointer
        alloc_info.descriptorPool = m_pool_stack.back();

        return { device.allocateDescriptorSets(alloc_info),
                 static_cast<uint32_t>(m_pool_stack.size() - 1) };

    } catch (const std::exception& e) {
        println("Failed to allocate descriptor sets {}", e.what());
        throw;
    }
}

void DescriptorPoolAllocator::deallocate(
    vk::Device        device,
    vk::DescriptorSet set,
    uint32_t          descriptor_pool_idx
) {
    device.freeDescriptorSets(m_pool_stack[descriptor_pool_idx], set);
}

void DescriptorPoolAllocator::reset(vk::Device device) {
    for (auto pool : m_pool_stack) {
        device.destroyDescriptorPool(pool);
    }
}

vk::DescriptorSetLayout DescriptorLayoutCache::get_or_create_layout(
    vk::Device                                        device,
    const std::vector<vk::DescriptorSetLayoutBinding> bindings
) {
    if (layout_cache.contains(bindings)) {
        return layout_cache[bindings];
    }

    vk::DescriptorSetLayoutCreateInfo layout_info{
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings    = bindings.data()
    };

    layout_cache[bindings] = device.createDescriptorSetLayout(layout_info);

    return layout_cache[bindings];
}

void DescriptorLayoutCache::reset(vk::Device device) {
    for (auto i : layout_cache) {
        device.destroyDescriptorSetLayout(i.second);
    }
    layout_cache.clear();
}

size_t LayoutKeyHash::operator()(
    const std::vector<vk::DescriptorSetLayoutBinding>& bindings
) const {
    size_t hash = 0;

    for (const auto& b : bindings) {
        hash ^= std::hash<uint32_t>{}(b.binding) + 0x9e3779b9 + (hash << 6) +
            (hash >> 2);

        hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(b.descriptorType)) +
            0x9e3779b9 + (hash << 6) + (hash >> 2);

        hash ^= std::hash<uint32_t>{}(b.descriptorCount) + 0x9e3779b9 +
            (hash << 6) + (hash >> 2);

        // hash ^= std::hash<vk::ShaderStageFlags>{}(b.stageFlags) +
        // 0x9e3779b9 +
        //     (hash << 6) + (hash >> 2);
    }

    return hash;
}
} // namespace varicle::renderer::vulkan

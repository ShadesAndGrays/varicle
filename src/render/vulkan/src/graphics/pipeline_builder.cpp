#include "graphics/pipeline.hpp"

namespace varicle::render::vulkan {

PipelineBuilder::PipelineBuilder() {}

PipelineBuilder& PipelineBuilder::set_vert_shader(
    vk::ShaderModule shader_module,
    std::string      entry_point
) {
    m_vert_shader_module = shader_module;
    m_vert_entry_point   = entry_point;

    return *this;
}

PipelineBuilder& PipelineBuilder::set_frag_shader(
    vk::ShaderModule shader_module,
    std::string      entry_point
) {

    m_frag_shader_module = shader_module;
    m_frag_entry_point   = entry_point;

    return *this;
}

PipelineBuilder& PipelineBuilder::set_vertex_input_info(
    std::vector<vk::VertexInputBindingDescription>   binding_descriptions,
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions
) {

    m_binding_descriptions = std::move(binding_descriptions);
    m_attribute_descriptions = std::move(attribute_descriptions);

    return *this;
}

PipelineBuilder& PipelineBuilder::set_pipeline_rendering(
    vk::Format color_format,
    vk::Format depth_format
) {
    m_color_format = color_format;
    m_depth_format = depth_format;

    return *this;
}

PipelineBuilder& PipelineBuilder::set_rasterizer(
    vk::PolygonMode      polygon_mode,
    vk::CullModeFlagBits cull_mode,
    vk::FrontFace        front_face
) {
    m_polygon_mode = polygon_mode;
    m_cull_mode    = cull_mode;
    m_front_face   = front_face;

    return *this;
}

PipelineBuilder& PipelineBuilder::set_multisampling(
    bool                    enable,
    vk::SampleCountFlagBits samples_flags
) {
    m_sample_enable = enable ? vk::True : vk::False;
    m_msaa_samples  = samples_flags;

    return *this;
}

PipelineBuilder& PipelineBuilder::set_blending(bool enable, bool logic_enable) {
    m_blending_enabled       = enable ? vk::True : vk::False;
    m_blending_logic_enabled = logic_enable ? vk::True : vk::False;

    return *this;
}

vk::Pipeline
PipelineBuilder::build(vk::Device& device, vk::PipelineLayout layout) {

    vk::PipelineShaderStageCreateInfo vert_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eVertex,
        .module = m_vert_shader_module,
        .pName  = m_vert_entry_point.c_str()
    };

    vk::PipelineShaderStageCreateInfo frag_shader_stageInfo{
        .stage  = vk::ShaderStageFlagBits::eFragment,
        .module = m_frag_shader_module,
        .pName  = m_frag_entry_point.c_str()
    };

    std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages{
        vert_shader_stageInfo,
        frag_shader_stageInfo,
    };

    vk::PipelineRenderingCreateInfo pipeline_rendering_create_info = {
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &m_color_format,
        .depthAttachmentFormat   = m_depth_format,

    };

    vk::PipelineViewportStateCreateInfo viewport_state{ .viewportCount = 1,
                                                        .scissorCount  = 1 };

    std::vector<vk::DynamicState> states = { vk::DynamicState::eViewport,
                                             vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynmaic_state = {
        .dynamicStateCount = static_cast<uint32_t>(states.size()),
        .pDynamicStates    = states.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo input_assembly{ .topology =
                                                                 m_topology };

    vk::PipelineVertexInputStateCreateInfo vertex_inputInfo{
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(m_binding_descriptions.size()),
        .pVertexBindingDescriptions = m_binding_descriptions.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(m_attribute_descriptions.size()),
        .pVertexAttributeDescriptions = m_attribute_descriptions.data()
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = m_polygon_mode,
        .cullMode                = m_cull_mode,
        .frontFace               = m_front_face,
        .depthBiasEnable         = vk::False,
        .lineWidth               = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = m_msaa_samples,
        .sampleShadingEnable  = m_sample_enable,
        .minSampleShading     = 0.2f

    };

    vk::PipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = m_blending_enabled,

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
        .logicOpEnable   = m_blending_logic_enabled,
        .logicOp         = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment
    };

    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{
        .stageCount          = static_cast<uint32_t>(shader_stages.size()),
        .pVertexInputState   = &vertex_inputInfo,
        .pInputAssemblyState = &input_assembly,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        // .pDepthStencilState  = &depth_stencil,
        .pColorBlendState    = &color_blending,
        .pDynamicState = &dynmaic_state,
        .layout        = layout,
        .renderPass    = nullptr
    };

    vk::StructureChain<
        vk::GraphicsPipelineCreateInfo,
        vk::PipelineRenderingCreateInfo>
        pipeline_create_info_chain = { graphics_pipeline_create_info,
                                       pipeline_rendering_create_info

        };

    if (auto pipeline = device.createGraphicsPipeline(
            nullptr, pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>()
        );
        pipeline.has_value()) {
        return pipeline.value;
    };

    throw std::runtime_error("Pipeline failed to be created!");
}
} // namespace varicle::render::vulkan

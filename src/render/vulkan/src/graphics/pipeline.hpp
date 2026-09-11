#include "graphics/resource.hpp"
#include <core/context.hpp>

namespace varicle::render::vulkan {

void create_descriptor_set_layout(VulkanContext& ctx);
void create_descriptor_pool(VulkanContext& ctx);
// void create_descriptor_set(VulkanContext& ctx);
void create_descriptor_set(VulkanContext& ctx, const Texture& texture);

void create_graphics_pipeline(VulkanContext& ctx);

[[nodiscard]] vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code);

class PipelineBuilder {

  private:
    // vk::PipelineDynamicStateCreateInfo m_dynmaic_state_create_info{
    //
    // };
    //
    vk::Format m_color_format;
    vk::Format m_depth_format;

    vk::ShaderModule m_frag_shader_module;
    vk::ShaderModule m_vert_shader_module;
    std::string      m_frag_entry_point;
    std::string      m_vert_entry_point;

    std::vector<vk::VertexInputBindingDescription>   m_binding_descriptions;
    std::vector<vk::VertexInputAttributeDescription> m_attribute_descriptions;

    vk::PrimitiveTopology m_topology = vk::PrimitiveTopology::eTriangleList;

    vk::PolygonMode      m_polygon_mode = vk::PolygonMode::eFill;
    vk::CullModeFlagBits m_cull_mode    = vk::CullModeFlagBits::eFront;
    vk::FrontFace        m_front_face   = vk::FrontFace::eClockwise;

    vk::Bool32 m_sample_enable;
    vk::SampleCountFlagBits m_msaa_samples;

    vk::Bool32 m_blending_enabled = vk::False;
    vk::Bool32 m_blending_logic_enabled = vk::False;
    // vk::PipelineRenderingCreateInfo m_pipeline_rendering_create_info {}

  public:
    PipelineBuilder();
    PipelineBuilder&
    set_vert_shader(vk::ShaderModule shader_module, std::string entry_point);

    PipelineBuilder&
    set_frag_shader(vk::ShaderModule shader_module, std::string entry_point);

    PipelineBuilder& set_vertex_input_info(
        std::vector<vk::VertexInputBindingDescription>   binding_descriptions,
        std::vector<vk::VertexInputAttributeDescription> attribute_descriptions
    );

    PipelineBuilder&
    set_pipeline_rendering(vk::Format color_format, vk::Format depth_format);

    PipelineBuilder& set_topology(vk::PrimitiveTopology topology);

    PipelineBuilder& set_rasterizer(
        vk::PolygonMode      polygon_mode,
        vk::CullModeFlagBits cull_mode,
        vk::FrontFace        front_face
    );

    PipelineBuilder& set_multisampling(
        bool                    enable,
        vk::SampleCountFlagBits samples_flags = vk::SampleCountFlagBits::e1
    );

    PipelineBuilder& set_blending(
        bool                    enable,
        bool                    logic_enable
    );

    PipelineBuilder& set_push_constant_size(
        bool                    enable,
        bool                    logic_enable
    );


    vk::Pipeline build(vk::Device& device, vk::PipelineLayout layout);
};
} // namespace varicle::render::vulkan

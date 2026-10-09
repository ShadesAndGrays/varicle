#pragma once
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/core/context.hpp"
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace varicle::renderer::vulkan {

[[nodiscard]] vk::ShaderModule create_shader_module(
    const VulkanContext&         ctx,
    const std::vector<uint32_t>& code
);

struct PipelineConfig {

  public:
    vk::Format color_format = vk::Format::eR32G32B32A32Sfloat;
    vk::Format depth_format = vk::Format::eD32Sfloat;

    vk::ShaderModule frag_shader_module;
    vk::ShaderModule vert_shader_module;
    const char*      frag_entry_point = "main";
    const char*      vert_entry_point = "main";

    std::vector<vk::VertexInputBindingDescription>   binding_descriptions;
    std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;

    vk::PolygonMode      polygon_mode = vk::PolygonMode::eFill;
    vk::CullModeFlagBits cull_mode    = vk::CullModeFlagBits::eFront;
    vk::FrontFace        front_face   = vk::FrontFace::eClockwise;

    vk::Bool32              sample_enable = vk::False;
    vk::SampleCountFlagBits msaa_samples  = vk::SampleCountFlagBits::e1;

    vk::Bool32 blending_enabled       = vk::False;
    vk::Bool32 blending_logic_enabled = vk::False;

    uint32_t                push_constant_range = sizeof(float);
    vk::ShaderStageFlagBits push_constant_stage =
        vk::ShaderStageFlagBits::eVertex;
};

// This will containt one pipeline and a descriptor set per frame
// It has its layout and and array of layout binding
struct PipelineBundle {
    // All other members are generated from layout bindings
    // buffers and images should are copied

    PipelineBundle(std::vector<vk::DescriptorSetLayout> layout)
        : descriptor_set_layout(layout)

    {}

    PipelineBundle(const PipelineBundle&)            = delete;
    PipelineBundle& operator=(const PipelineBundle&) = delete;

    PipelineBundle(PipelineBundle&&)            = default;
    PipelineBundle& operator=(PipelineBundle&&) = default;

    // Descriptors Attributes
    std::vector<vk::DescriptorSetLayout> descriptor_set_layout;

    // Pipeline Attributes
    vk::Pipeline       pipeline;
    vk::PipelineLayout pipeline_layout;
};

void create_graphics_pipeline(
    vk::Device      device,
    PipelineConfig  config,
    PipelineBundle& bundle
);

struct LayoutKeyHash {
    size_t operator()(
        const std::vector<vk::DescriptorSetLayoutBinding>& bindings
    ) const;
};

class DescriptorLayoutCache {
  private:
    std::unordered_map<
        std::vector<vk::DescriptorSetLayoutBinding>,
        vk::DescriptorSetLayout,
        LayoutKeyHash>
        layout_cache;

  public:
    vk::DescriptorSetLayout get_or_create_layout(
        vk::Device                                        device,
        const std::vector<vk::DescriptorSetLayoutBinding> bindings
    );
};

class DescriptorPoolAllocator {
  private:
    // some default setting
    static constexpr int s_maxSets  = 256;
    static constexpr int s_samplers = 1024;
    static constexpr int s_buffers  = 256;

    std::vector<vk::DescriptorPool> m_full;
    vk::DescriptorPool              m_current = nullptr;

  public:
    void create_new_pool(vk::Device device);

    std::vector<vk::DescriptorSet> allocate(
        vk::Device              device,
        std::vector<vk::DescriptorSetLayout> layouts
    );
};

class DescriptorWriter {
  public:
    void write_buffer(
        vk::Device                                device,
        vk::DescriptorSetLayoutBinding            layout_binding,
        vk::DescriptorSet                         descriptor_set,
        std::span<const vk::DescriptorBufferInfo> buffer_infos,
        std::uint32_t                             offset = 0

    );

    void write_texture(
        vk::Device                         device,
        vk::DescriptorSetLayoutBinding     layout_binding,
        vk::DescriptorSet                  descriptor_set,
        std::span<const vk::DescriptorImageInfo> image_infos,
        std::uint32_t                      offset = 0
    );
};

struct DescriptorSystem {
    DescriptorLayoutCache   cache;
    DescriptorPoolAllocator pool_allocator;
    DescriptorWriter        writer;
};

DescriptorSystem create_default_descriptor_system(vk::Device device);

} // namespace varicle::renderer::vulkan

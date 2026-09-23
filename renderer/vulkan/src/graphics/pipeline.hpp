#include <core/config.hpp>
#include <core/context.hpp>
#include <unordered_map>

namespace varicle::render::vulkan {

[[nodiscard]] vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code);

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

    PipelineBundle(
        std::vector<vk::DescriptorSetLayoutBinding> layout_bindings,
        std::vector<vk::DescriptorImageInfo>        associated_images,
        std::array<std::vector<vk::DescriptorBufferInfo>, MAX_FRAMES_IN_FLIGHT>
            associated_buffers
    )
        : descriptor_set_layout_bindings(layout_bindings),
          descriptor_images(associated_images)

    {}

    PipelineBundle(const PipelineBundle&)            = delete;
    PipelineBundle& operator=(const PipelineBundle&) = delete;

    PipelineBundle(PipelineBundle&&)            = default;
    PipelineBundle& operator=(PipelineBundle&&) = default;

    // Descriptors Attributes
    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_sets;
    vk::DescriptorSetLayout                             descriptor_set_layout;
    std::vector<vk::DescriptorSetLayoutBinding> descriptor_set_layout_bindings;
    std::vector<vk::DescriptorImageInfo>        descriptor_images;
    std::vector<vk::DescriptorBufferInfo>       descriptor_buffers;

    // Pipeline Attributes
    vk::Pipeline       pipeline;
    vk::PipelineLayout pipeline_layout;
};

// A more stateful approach to pipeline creation
class PipelineCreationSystem {

    /*
     * Clear system state

     * Add Layout binding for set - increments the set
     * Run Create Layout - Creates all layouts from bindings

     * Run Create Pool - Create pool using aggregated binding infromatino

     // To complex to automate here
     * Run Create Sets - Use layout and pool to create sets

     * Create Pipeline bundle
     * Destroy Layouts
     * (Optional) Destroy pool if no longer needed
     * Return Pipeline Bundle containing Pipeline and Descriptor Set
     */

  private:
    // For pool creation
    // The total number of descriptor sets for single frame
    uint32_t m_descriptor_set_count;
    // A combination of all layouts
    std::unordered_map<vk::DescriptorType, uint32_t> m_combined_layouts;

  public:
    void create_graphics_pipeline(
        vk::Device      device,
        PipelineConfig  config,
        PipelineBundle& bundle
    );

    // Inplace create descriptors bundles. Each bundle should have the layout
    // binding and is associates image or buffer
    void
    build_descriptors(vk::Device device, std::vector<PipelineBundle>& bundles);

  private:
    // add bindings to combined layout set
    // increment descriptor_set_count by 1
    // User should take layout bundle it
    vk::DescriptorSetLayout create_descriptor_set_layout(
        vk::Device                                         device,
        const std::vector<vk::DescriptorSetLayoutBinding>& layout_binding
    );

    // This uses information tracked by create_descriptor_set_layout to create a
    // pool
    vk::DescriptorPool calculate_descriptor_pool(vk::Device device);

    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> create_descriptor_set(
        vk::Device              device,
        vk::DescriptorSetLayout layout,
        vk::DescriptorPool      pool
    );

    std::vector<vk::WriteDescriptorSet>
    create_descriptor_writes(PipelineBundle& bundle);

    void apply_writes(
        vk::Device                                 device,
        const std::vector<vk::WriteDescriptorSet>& writes
    );
};

} // namespace varicle::render::vulkan

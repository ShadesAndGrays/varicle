#pragma once
#include "renderer/common/handles.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/buffer.hpp"
#include <vulkan/vulkan.hpp>

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace varicle::renderer::vulkan {

static std::vector<vk::VertexInputAttributeDescription>
getAttributeDescriptions() {
    return { {
        { .location = 0,
          .binding  = 0,
          .format   = vk::Format::eR32G32B32Sfloat,
          .offset   = offsetof(Vertex, position) },
        { .location = 1,
          .binding  = 0,
          .format   = vk::Format::eR32G32B32Sfloat,
          .offset   = offsetof(Vertex, normal) },
        { .location = 2,
          .binding  = 0,
          .format   = vk::Format::eR32G32Sfloat,
          .offset   = offsetof(Vertex, uv) },
    } };
}

static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions() {
    return { { .binding   = 0,
               .stride    = sizeof(Vertex),
               .inputRate = vk::VertexInputRate::eVertex } };
}

template <typename T> struct UniformBufferData {
    UniformBufferData() = default;

    UniformBufferData(const UniformBufferData&)            = delete;
    UniformBufferData& operator=(const UniformBufferData&) = delete;

    UniformBufferData(UniformBufferData&& other) noexcept
        : m_uniform_buffers(std::move(other.m_uniform_buffers)),
          m_uniform_buffers_memory(std::move(other.m_uniform_buffers_memory)),
          m_uniform_buffers_mapped(std::move(other.m_uniform_buffers_mapped)) {
        other.m_uniform_buffers_mapped.clear();
    }

    UniformBufferData& operator=(UniformBufferData&& other) noexcept {
        if (this != &other) {
            m_uniform_buffers = std::move(other.m_uniform_buffers);
            m_uniform_buffers_memory =
                std::move(other.m_uniform_buffers_memory);
            m_uniform_buffers_mapped =
                std::move(other.m_uniform_buffers_mapped);
            other.m_uniform_buffers_mapped.clear();
        }
        return *this;
    }

    std::vector<vk::Buffer>       m_uniform_buffers;
    std::vector<vk::DeviceMemory> m_uniform_buffers_memory;
    std::vector<void*>            m_uniform_buffers_mapped;

    void update_uniform_buffer(const T& ubo, uint32_t idx) {
        assert(idx < m_uniform_buffers_mapped.size() && "Index out of bounds!");
        memcpy(m_uniform_buffers_mapped[idx], &ubo, sizeof(ubo));
    }

    vk::DescriptorBufferInfo get_buffer_info(uint32_t idx) {
        return vk::DescriptorBufferInfo{
            m_uniform_buffers[idx],
            0,
            sizeof(T),
        };
    }

    static UniformBufferData create_uniform_buffer(VulkanContext& ctx) {

        UniformBufferData g_buffer{};

        std::array<uint32_t, 1> qf{ ctx.m_indices.graphics_family.value() };

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::DeviceSize buffer_size = sizeof(T);

            auto [buffer, buffer_memory] = create_buffer(
                ctx,
                buffer_size,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible |
                    vk::MemoryPropertyFlagBits::eHostCoherent,
                qf
            );
            g_buffer.m_uniform_buffers.push_back(buffer);
            g_buffer.m_uniform_buffers_memory.push_back(buffer_memory);
            g_buffer.m_uniform_buffers_mapped.push_back(ctx.m_device.mapMemory(
                g_buffer.m_uniform_buffers_memory.back(), 0, buffer_size
            ));
        }
        return g_buffer;
    }

    static void
    destroy_uniform_buffer(vk::Device device, UniformBufferData g_buffer) {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.freeMemory(g_buffer.m_uniform_buffers_memory[i]);
            device.destroyBuffer(g_buffer.m_uniform_buffers[i]);
        }
    }
};

struct TextureData {

    TextureHandle           ID;
    uint32_t                m_mip_levels   = 0;
    vk::Image               m_image        = nullptr;
    vk::ImageView           m_image_view   = nullptr;
    vk::DeviceMemory        m_image_memory = nullptr;
    vk::DescriptorImageInfo m_descriptor_image_info{};

    TextureData()  = default;
    ~TextureData() = default;

    TextureData(const TextureData&)            = delete;
    TextureData& operator=(const TextureData&) = delete;

    TextureData(TextureData&&) noexcept            = default;
    TextureData& operator=(TextureData&&) noexcept = default;

    TextureData(vk::Image image) : m_image(image) {}
};

struct MeshData {
    // loaded on the cpu

    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;

    // referenct to the gpu
    vk::Buffer m_vertex_buffer = nullptr; // interface for memory
    vk::Buffer m_index_buffer  = nullptr; // interface for memory

    // memory on the gpu
    vk::DeviceMemory m_vertex_buffer_memory = nullptr; // actually memory
    vk::DeviceMemory m_index_buffer_memory  = nullptr; // actually memory

    MeshData()  = default;
    ~MeshData() = default;

    MeshData(const MeshData&)            = delete;
    MeshData& operator=(const MeshData&) = delete;

    MeshData(MeshData&&) noexcept            = default;
    MeshData& operator=(MeshData&&) noexcept = default;

    MeshData(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}

    static void create_vertex_buffer(VulkanContext& ctx, MeshData& mesh);
    static void create_index_buffer(VulkanContext& ctx, MeshData& mesh);
};

struct MaterialData {

    MaterialData(
        vk::DescriptorSetLayout descriptor_set_layout,
        vk::PipelineLayout      pipeline_layout,
        vk::Pipeline            graphics_pipeline,
        vk::DescriptorSet       descriptor_set,
        uint32_t                descriptor_pool_idx,
        ShaderHandle            vertex_shader   = INVALID_SHADER,
        ShaderHandle            fragment_shader = INVALID_SHADER
    )
        : m_descriptor_set_layout(std::move(descriptor_set_layout)),
          m_graphics_pipeline(std::move(graphics_pipeline)),
          m_pipeline_layout(std::move(pipeline_layout)),
          m_descriptor_set(std::move(descriptor_set)),
          m_descriptor_pool_idx(descriptor_pool_idx),
          m_vertex_shader(vertex_shader), m_fragment_shader(fragment_shader) {}

    MaterialData(const MaterialData&)            = delete;
    MaterialData& operator=(const MaterialData&) = delete;

    MaterialData(MaterialData&&)            = default;
    MaterialData& operator=(MaterialData&&) = default;

    vk::DescriptorSetLayout m_descriptor_set_layout = nullptr;
    vk::PipelineLayout      m_pipeline_layout       = nullptr;
    vk::Pipeline            m_graphics_pipeline     = nullptr;
    vk::DescriptorSet       m_descriptor_set;
    uint32_t                m_descriptor_pool_idx;
    ShaderHandle            m_vertex_shader   = INVALID_SHADER;
    ShaderHandle            m_fragment_shader = INVALID_SHADER;
};

} // namespace varicle::renderer::vulkan

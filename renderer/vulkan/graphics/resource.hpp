#pragma once
#include "renderer/common/handles.hpp"
#include "renderer/common/vertex.hpp"
#include "vulkan/vulkan.hpp"

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

// struct VVertex {
//     glm::vec3 pos;
//     glm::vec3 color;
//     glm::vec2 tex_coord;
//
//     static std::vector<vk::VertexInputBindingDescription>
//     getBindingDescriptions() {
//         return { { .binding   = 0,
//                    .stride    = sizeof(VVertex),
//                    .inputRate = vk::VertexInputRate::eVertex } };
//     }
//
//     static std::vector<vk::VertexInputAttributeDescription>
//     getAttributeDescriptions() {
//         return { {
//             { .location = 0,
//               .binding  = 0,
//               .format   = vk::Format::eR32G32B32Sfloat,
//               .offset   = offsetof(VVertex, pos) },
//             { .location = 1,
//               .binding  = 0,
//               .format   = vk::Format::eR32G32B32Sfloat,
//               .offset   = offsetof(VVertex, color) },
//             { .location = 2,
//               .binding  = 0,
//               .format   = vk::Format::eR32G32Sfloat,
//               .offset   = offsetof(VVertex, tex_coord) },
//         } };
//     }
//
//     bool operator==(const VVertex& other) const {
//         return pos == other.pos && color == other.color &&
//             tex_coord == other.tex_coord;
//     }
// };
//
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

// Strucutre for managing mesh level transfromations
// Stick with simple oop for now consider SOA for future optimizations
// struct Object {
//     glm::vec3 position{ 0.0f, 0.0f, 0.0f };
//     glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
//     glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
//
//     MeshHandle     mesh;
//     TextureHandle  texture;
//     MaterialHandle material;
//
//     glm::mat4 get_model_matrix() const {
//         glm::mat4 model = glm::mat4(1.0f); // identity
//         model           = glm::translate(model, position);
//         model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//         model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//         model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//         model = glm::scale(model, scale);
//
//         return model;
//     }
// };

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

    MeshHandle            ID;
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
};

struct MaterialData {

    MaterialData(
        vk::DescriptorSetLayout        descriptor_set_layout,
        vk::PipelineLayout             pipeline_layout,
        vk::Pipeline                   graphics_pipeline,
        std::vector<vk::DescriptorSet> descriptor_sets
    )
        : m_descriptor_set_layout(std::move(descriptor_set_layout)),
          m_graphics_pipeline(std::move(graphics_pipeline)),
          m_pipeline_layout(std::move(pipeline_layout)),
          m_descriptor_sets(std::move(descriptor_sets)) {}

    MaterialData(const MaterialData&)            = delete;
    MaterialData& operator=(const MaterialData&) = delete;

    MaterialData(MaterialData&&)            = default;
    MaterialData& operator=(MaterialData&&) = default;

    vk::DescriptorSetLayout        m_descriptor_set_layout = nullptr;
    vk::PipelineLayout             m_pipeline_layout       = nullptr;
    vk::Pipeline                   m_graphics_pipeline     = nullptr;
    std::vector<vk::DescriptorSet> m_descriptor_sets;
};

} // namespace varicle::renderer::vulkan

// namespace std {
// template <> struct hash<varicle::renderer::vulkan::VVertex> {
//     size_t operator()(varicle::renderer::vulkan::VVertex const& vertex) const
//     {
//         return ((hash<glm::vec3>()(vertex.pos) ^
//                  (hash<glm::vec3>()(vertex.color) << 1)) >>
//                 1) ^
//             (hash<glm::vec2>()(vertex.tex_coord) << 1);
//     }
// };
// } // namespace std

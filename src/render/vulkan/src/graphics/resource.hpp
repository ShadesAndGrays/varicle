#pragma once
#include "handles.hpp"
#include <vulkan/vulkan.hpp>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace varicle::render::vulkan {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex_coord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return { .binding   = 0,
                 .stride    = sizeof(Vertex),
                 .inputRate = vk::VertexInputRate::eVertex };
    }

    static std::array<vk::VertexInputAttributeDescription, 3>
    getAttributeDescriptions() {
        return { {
            { .location = 0,
              .binding  = 0,
              .format   = vk::Format::eR32G32B32Sfloat,
              .offset   = offsetof(Vertex, pos) },
            { .location = 1,
              .binding  = 0,
              .format   = vk::Format::eR32G32B32Sfloat,
              .offset   = offsetof(Vertex, color) },
            { .location = 2,
              .binding  = 0,
              .format   = vk::Format::eR32G32Sfloat,
              .offset   = offsetof(Vertex, tex_coord) },
        } };
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color &&
            tex_coord == other.tex_coord;
    }
};

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

struct Texture {

    uint32_t         m_mip_levels   = 0;
    vk::Image        m_image        = nullptr;
    vk::ImageView    m_image_view   = nullptr;
    vk::DeviceMemory m_image_memory = nullptr;

    Texture()  = default;
    ~Texture() = default;

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) noexcept            = default;
    Texture& operator=(Texture&&) noexcept = default;

    Texture(vk::Image image) : m_image(image) {}
};

struct Mesh {
    // loaded on the cpu
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;

    // referenct to the gpu
    vk::Buffer m_vertex_buffer = nullptr; // interface for memory
    vk::Buffer m_index_buffer  = nullptr; // interface for memory

    // acutal memory on the gpu
    vk::DeviceMemory m_vertex_buffer_memory = nullptr; // actually memory
    vk::DeviceMemory m_index_buffer_memory  = nullptr; // actually memory
                                                       //

    Mesh()  = default;
    ~Mesh() = default;

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept            = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}
};

struct Material {

    vk::DescriptorSetLayout        m_descriptor_set_layout = nullptr;
    vk::PipelineLayout             m_pipeline_layout       = nullptr;
    vk::Pipeline                   m_graphics_pipeline     = nullptr;
    vk::ShaderModule               m_shader_module         = nullptr;
    vk::DescriptorPool             m_descriptor_pool       = nullptr;
    std::vector<vk::DescriptorSet> m_descriptor_sets;
};


} // namespace varicle::render::vulkan

namespace std {
template <> struct hash<varicle::render::vulkan::Vertex> {
    size_t operator()(varicle::render::vulkan::Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
                 (hash<glm::vec3>()(vertex.color) << 1)) >>
                1) ^
            (hash<glm::vec2>()(vertex.tex_coord) << 1);
    }
};
} // namespace std

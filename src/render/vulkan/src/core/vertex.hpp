#pragma once
#include "glm/glm.hpp"
#include <vulkan/vulkan_raii.hpp>

struct Vertex {
    glm::vec2 pos;
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
              .format   = vk::Format::eR32G32Sfloat,
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
};

struct UniformBufferObject {
    // glm::vec2 uv;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    // alignas(16) glm::mat4 model;
    // alignas(16) glm::mat4 view;
    // alignas(16) glm::mat4 proj;
};

const std::vector<Vertex> vertices{
    { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
};

const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };

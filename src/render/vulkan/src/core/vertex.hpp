#pragma once
#include <vulkan/vulkan_raii.hpp>
#include"glm/glm.hpp"


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return { .binding   = 0,
                 .stride    = sizeof(Vertex),
                 .inputRate = vk::VertexInputRate::eVertex };
    }

    static std::array<vk::VertexInputAttributeDescription, 2>
    getAttributeDescriptions() {
        return { { { .location = 0,
                     .binding  = 0,
                     .format   = vk::Format::eR32G32Sfloat,
                     .offset   = offsetof(Vertex, pos) },
                   { .location = 1,
                     .binding  = 0,
                     .format   = vk::Format::eR32G32B32Sfloat,
                     .offset   = offsetof(Vertex, color) } } };
    }
};

const std::vector<Vertex> vertices{
    { { 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.7f, 0.7f }, { 0.0f, 0.0f, 1.0f } },
};


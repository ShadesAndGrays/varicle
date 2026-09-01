#pragma once

#include "vertex.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

struct Mesh {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;

    vk::Buffer       vertex_buffer        = nullptr;
    vk::DeviceMemory vertex_buffer_memory = nullptr;

    vk::Buffer       index_buffer        = nullptr;
    vk::DeviceMemory index_buffer_memory = nullptr;
};

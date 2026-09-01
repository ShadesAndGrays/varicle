#pragma once
#include "core/context.hpp"
#include <utility>

namespace varicle::render::vulkan {

void create_command_pool(VulkanContext& ctx);

std::pair<vk::Buffer, vk::DeviceMemory> create_buffer(
    VulkanContext&          ctx,
    vk::DeviceSize          size,
    vk::BufferUsageFlags    usage,
    vk::MemoryPropertyFlags properties,
    std::span<uint32_t>     queue_family
);

uint32_t find_memory_type(
    VulkanContext&          ctx,
    uint32_t                typeFilter,
    vk::MemoryPropertyFlags properties
);

void create_vertex_buffer(VulkanContext& ctx);
void create_index_buffer(VulkanContext& ctx);
void create_uniform_buffer(VulkanContext& ctx);
void update_uniform_buffer(VulkanContext& ctx);

void create_command_buffers(VulkanContext& ctx);

void create_sync_objects(VulkanContext& ctx);
} // namespace varicle::render::vulkan

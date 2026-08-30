#pragma once
#include "core/context.hpp"

namespace varicle::render::vulkan {

void create_command_pool(VulkanContext& ctx);
void create_vertex_buffer(VulkanContext& ctx);

void create_command_buffers(VulkanContext& ctx);

void create_sync_objects(VulkanContext& ctx);
} // namespace varicle::render::vulkan

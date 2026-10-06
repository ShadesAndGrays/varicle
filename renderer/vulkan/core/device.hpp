#pragma once
#include "renderer/vulkan/core/context.hpp"

namespace varicle::renderer::vulkan {

void select_physical_device(VulkanContext& ctx);
void create_logical_device(VulkanContext& ctx);
bool is_device_suitable(vk::PhysicalDevice const& physical_device);
void list_devices(VulkanContext& ctx);

QueueFamilyIndices
find_queue_families_indices(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface);

} // namespace varicle::render::vulkan

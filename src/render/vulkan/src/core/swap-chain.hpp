#pragma once
#include "core/context.hpp"

namespace varicle::render::vulkan {

void create_swap_chain(VulkanContext& ctx);

uint32_t choose_swap_min_image_count(
    vk::SurfaceCapabilitiesKHR const& surfaceCapabilities
);

vk::SurfaceFormatKHR choose_swap_surface_format(
    std::vector<vk::SurfaceFormatKHR> const& availableFormats
);

vk::PresentModeKHR choose_swap_present_mode(
    std::vector<vk::PresentModeKHR> const& availablePresentModes
);

vk::Extent2D choose_swap_extent(
    VulkanContext&                    ctx,
    vk::SurfaceCapabilitiesKHR const& capabilities
);

void create_image_views(VulkanContext& ctx);

void cleanup_swap_chain(VulkanContext& ctx);

void recreate_swap_chain(VulkanContext& ctx);

} // namespace varicle::render::vulkan

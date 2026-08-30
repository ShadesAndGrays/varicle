#pragma once
#include "core/context.hpp"

namespace varicle::render::vulkan {

void init_window(
    VulkanContext& ctx,
    uint32_t       width  = 800,
    uint32_t       height = 600,
    const char*    name   = "Varicle"
);

vk::ApplicationInfo create_app();

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*>& validation_layers,
    vk::ApplicationInfo*            p_app_info,
    std::vector<const char*>&       required_layers,
    std::vector<const char*>&       required_extensions
);

void create_instance(VulkanContext& ctx);
void create_surface(VulkanContext& ctx);

} // namespace varicle::render::vulkan

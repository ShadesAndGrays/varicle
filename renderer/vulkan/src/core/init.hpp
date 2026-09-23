#pragma once
#include "core/context.hpp"
#include "common.hpp"

namespace varicle::render::vulkan {


vk::ApplicationInfo create_app();

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*>& validation_layers,
    vk::ApplicationInfo*            p_app_info,
    std::vector<const char*>&       required_layers,
    std::vector<const char*>&       required_extensions
);

void create_instance(VulkanContext& ctx);
void create_surface(VulkanContext& ctx,Window &window);

} // namespace varicle::render::vulkan

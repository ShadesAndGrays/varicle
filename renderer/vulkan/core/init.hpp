#pragma once
#include "renderer/vulkan/core/context.hpp"
#include "renderer/common/common.hpp"

namespace varicle::renderer::vulkan {


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

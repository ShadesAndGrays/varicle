#pragma once
#include "core/context.hpp"
#include "vulkan-renderer.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {
void transition_image_layout(
    VulkanContext&          ctx,
    uint32_t                imageIndex,
    vk::ImageLayout         old_layout,
    vk::ImageLayout         new_layout,
    vk::AccessFlags2        src_access_mask,
    vk::AccessFlags2        dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask
);
} // namespace varicle::render::vulkan

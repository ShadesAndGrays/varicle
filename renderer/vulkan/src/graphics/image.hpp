#pragma once

#include "core/context.hpp"
#include "graphics/resource.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {


void transition_image_layout(
    VulkanContext&          ctx,
    vk::Image               image,
    vk::ImageLayout         old_layout,
    vk::ImageLayout         new_layout,
    vk::AccessFlags2        src_access_mask,
    vk::AccessFlags2        dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask,
    vk::ImageAspectFlags    image_aspect_flags

);

void transition_image_layout(
    vk::CommandBuffer command_buffer,
    vk::Image         image,
    vk::ImageLayout   old_layout,
    vk::ImageLayout   new_layout,
    uint32_t          mip_levels
);

void          create_texture_image_view(VulkanContext& ctx);
vk::ImageView create_image_view(
    VulkanContext&       ctx,
    vk::Image const&     image,
    vk::Format           format,
    vk::ImageAspectFlags aspect_flag,
    uint32_t             mip_levels
);

std::pair<vk::Image, vk::DeviceMemory> create_image(
    VulkanContext&          ctx,
    uint32_t                width,
    uint32_t                height,
    uint32_t                mip_levels,
    vk::SampleCountFlagBits num_samples,
    vk::Format              format,
    vk::ImageTiling         tiling,
    vk::ImageUsageFlags     usage,
    vk::MemoryPropertyFlags propreties
);

void copy_buffer_to_image(
    vk::CommandBuffer command_buffer,
    const vk::Buffer& buffer,
    vk::Image         image,
    uint32_t          width,
    uint32_t          height
);

void generate_mipmaps(
    VulkanContext&     ctx,
    vk::CommandBuffer& command_buffer,
    vk::Image          image,
    vk::Format         image_format,
    int32_t            texture_width,
    int32_t            texture_height,
    uint32_t           mip_levels
);


void create_texture_sampler(VulkanContext& ctx);
void create_depth_resources(VulkanContext& ctx);
void cleanup_depth_resources(VulkanContext& ctx);

void create_color_resources(VulkanContext& ctx);
void cleanup_color_resources(VulkanContext& ctx);

} // namespace varicle::render::vulkan

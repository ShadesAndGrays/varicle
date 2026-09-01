#include "image.hpp"
#include "buffer.hpp"
#include "core/context.hpp"
#include "model/model.hpp"
#include "util/command.hpp"
#include <vulkan/vulkan.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
) {
    vk::ImageMemoryBarrier2 barrier{
        .srcStageMask        = src_stage_mask,
        .srcAccessMask       = src_access_mask,
        .dstStageMask        = dst_stage_mask,
        .dstAccessMask       = dst_access_mask,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange    = { .aspectMask     = image_aspect_flags,
                                 .baseMipLevel   = 0,
                                 .levelCount     = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount     = 1 }
    };

    vk::DependencyInfo dependency_info = { .dependencyFlags         = {},
                                           .imageMemoryBarrierCount = 1,
                                           .pImageMemoryBarriers = &barrier };
    ctx.m_command_buffers[ctx.m_frame_index].pipelineBarrier2(dependency_info);
}

std::pair<vk::Image, vk::DeviceMemory> create_image(
    VulkanContext&          ctx,
    uint32_t                width,
    uint32_t                height,
    vk::Format              format,
    vk::ImageTiling         tiling,
    vk::ImageUsageFlags     usage,
    vk::MemoryPropertyFlags propreties
) {
    vk::ImageCreateInfo image_info{
        .imageType     = vk::ImageType::e2D,
        .format        = format,
        .extent        = { width, height, 1 },
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = tiling,
        .usage         = usage,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    vk::Image              image = ctx.m_device.createImage(image_info);
    vk::MemoryRequirements mem_requirements =
        ctx.m_device.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo alloc_info{
        .allocationSize = mem_requirements.size,
        .memoryTypeIndex =
            find_memory_type(ctx, mem_requirements.memoryTypeBits, propreties)
    };
    vk::DeviceMemory image_memory = ctx.m_device.allocateMemory(alloc_info);
    ctx.m_device.bindImageMemory(image, image_memory, 0);

    return { image, image_memory };
}

/* This loads the texture into ram
 * Creates a staging buffer to transfer to the GPU
 * Then transfers the image and crates it
 */
void create_texture_image(VulkanContext& ctx) {
    if (ctx.m_command_pool == nullptr) {
        throw std::runtime_error(
            "Can't create image without a graphics-enabled command pool"
        );
    }

    int texture_width, texture_height, texture_channels;

    stbi_uc* pixels = stbi_load(
        TEXTURE_PATH.c_str(),
        &texture_width,
        &texture_height,
        &texture_channels,
        STBI_rgb_alpha
    );

    vk::DeviceSize image_buffer_size =
        texture_width * texture_height * 4; // 4 channels rgba;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    uint32_t qfp[] = { ctx.m_indices.graphics_family.value() };
    auto [staging_buffer, staging_buffer_memory] = create_buffer(
        ctx,
        image_buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        qfp
    );

    void* data =
        ctx.m_device.mapMemory(staging_buffer_memory, 0, image_buffer_size);
    memcpy(data, pixels, image_buffer_size);
    stbi_image_free(pixels);

    std::tie(ctx.m_texture_image, ctx.m_texture_image_memory) = create_image(
        ctx,
        texture_width,
        texture_height,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    vk::CommandBuffer temp_cmd_buffer = begin_single_time_commands(ctx);
    transition_image_layout(
        temp_cmd_buffer,
        ctx.m_texture_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );
    copy_buffer_to_image(
        temp_cmd_buffer,
        staging_buffer,
        ctx.m_texture_image,
        static_cast<uint32_t>(texture_width),
        static_cast<uint32_t>(texture_height)
    );
    transition_image_layout(
        temp_cmd_buffer,
        ctx.m_texture_image,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    end_single_time_commands(ctx, temp_cmd_buffer);

    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);
}

void transition_image_layout(
    vk::CommandBuffer command_buffer,
    vk::Image         image,
    vk::ImageLayout   old_layout,
    vk::ImageLayout   new_layout
) {
    vk::ImageMemoryBarrier barrier{

        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange    = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .levelCount = 1,
                                 .layerCount = 1 }
    };

    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;

    if (old_layout == vk::ImageLayout::eUndefined &&
        new_layout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eTransfer;
    } else if (
        old_layout == vk::ImageLayout::eTransferDstOptimal &&
        new_layout == vk::ImageLayout::eShaderReadOnlyOptimal
    ) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_stage      = vk::PipelineStageFlagBits::eTransfer;
        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("Unsupported layout transition");
    }

    command_buffer.pipelineBarrier(
        source_stage, destination_stage, {}, {}, nullptr, barrier
    );
    // {});
}
// Images should already be in Transfer Destination optimal to take the
// information form the buffer
void copy_buffer_to_image(
    vk::CommandBuffer command_buffer,
    const vk::Buffer& buffer,
    vk::Image         image,
    uint32_t          width,
    uint32_t          height
) {
    vk::BufferImageCopy region{
        .bufferOffset     = 0,
        .bufferRowLength  = 0,
        .imageSubresource = { .aspectMask     = vk::ImageAspectFlagBits::eColor,
                              .mipLevel       = 0,
                              .baseArrayLayer = 0,
                              .layerCount     = 1 },
        .imageOffset      = { 0, 0, 0 },
        .imageExtent      = { width, height, 1 }
    };

    command_buffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, region
    );
}

vk::ImageView create_image_view(
    VulkanContext&       ctx,
    vk::Image const&     image,
    vk::Format           format,
    vk::ImageAspectFlags aspect_flag
) {
    vk::ImageViewCreateInfo view_info{

        .image            = image,
        .viewType         = vk::ImageViewType::e2D,
        .format           = format,
        .subresourceRange = { .aspectMask     = aspect_flag,
                              .baseMipLevel   = 0,
                              .levelCount     = 1,
                              .baseArrayLayer = 0,
                              .layerCount     = 1 }
    };
    return ctx.m_device.createImageView(view_info);
}

void create_texture_image_view(VulkanContext& ctx) {

    ctx.m_texture_image_view = create_image_view(
        ctx,
        ctx.m_texture_image,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor
    );
}

void create_texture_sampler(VulkanContext& ctx) {
    vk::PhysicalDeviceProperties properties =
        ctx.m_physical_device.getProperties();
    vk::SamplerCreateInfo sampler_info{
        .magFilter        = vk::Filter::eLinear,
        .minFilter        = vk::Filter::eLinear,
        .mipmapMode       = vk::SamplerMipmapMode::eLinear,
        .addressModeU     = vk::SamplerAddressMode::eRepeat,
        .addressModeV     = vk::SamplerAddressMode::eRepeat,
        .addressModeW     = vk::SamplerAddressMode::eRepeat,
        .anisotropyEnable = vk::True,
        .maxAnisotropy    = properties.limits.maxSamplerAnisotropy,
        .compareEnable    = vk::False,
        .compareOp        = vk::CompareOp::eAlways
    };

    ctx.m_texture_sampler = ctx.m_device.createSampler(sampler_info);
}

vk::Format find_supported_format(
    VulkanContext&                 ctx,
    const std::vector<vk::Format>& candidates,
    vk::ImageTiling                tiling,
    vk::FormatFeatureFlags         features
) {
    for (const auto format : candidates) {
        vk::FormatProperties props =
            ctx.m_physical_device.getFormatProperties(format);

        if (((tiling == vk::ImageTiling::eLinear) &&
             ((props.linearTilingFeatures & features) == features)) ||
            ((tiling == vk::ImageTiling::eOptimal) &&
             ((props.optimalTilingFeatures & features) == features))) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

vk::Format find_depth_format(VulkanContext& ctx) {
    return find_supported_format(
        ctx,
        { vk::Format::eD32Sfloat,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void cleanup_depth_resources(VulkanContext& ctx) {
    if (ctx.m_depth_image_memory) {
        ctx.m_device.freeMemory(ctx.m_depth_image_memory);
        ctx.m_depth_image_memory = nullptr;
    }
    if (ctx.m_depth_image) {
        ctx.m_device.destroyImage(ctx.m_depth_image);
        ctx.m_depth_image = nullptr;
    }
    if (ctx.m_depth_image_view) {
        ctx.m_device.destroyImageView(ctx.m_depth_image_view);
        ctx.m_depth_image_view = nullptr;
    }
}

void create_depth_resources(VulkanContext& ctx) {
    vk::Format depth_format = find_depth_format(ctx);
    std::tie(ctx.m_depth_image, ctx.m_depth_image_memory) = create_image(
        ctx,
        ctx.m_swap_chain_extent.width,
        ctx.m_swap_chain_extent.height,
        depth_format,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ctx.m_depth_image_view = create_image_view(
        ctx, ctx.m_depth_image, depth_format, vk::ImageAspectFlagBits::eDepth
    );
    ctx.m_depth_format = depth_format;
}

} // namespace varicle::render::vulkan

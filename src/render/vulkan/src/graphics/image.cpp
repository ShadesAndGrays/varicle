#include "image.hpp"
#include "buffer.hpp"
#include "core/context.hpp"
#include "model/model.hpp"
#include "util/command.hpp"
#include <vulkan/vulkan.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <print>
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
    uint32_t                mip_levels,
    vk::SampleCountFlagBits num_samples,
    vk::Format              format,
    vk::ImageTiling         tiling,
    vk::ImageUsageFlags     usage,
    vk::MemoryPropertyFlags propreties
) {
    vk::ImageCreateInfo image_info{
        .imageType     = vk::ImageType::e2D,
        .format        = format,
        .extent        = { width, height, 1 },
        .mipLevels     = mip_levels,
        .arrayLayers   = 1,
        .samples       = num_samples,
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

void generate_mipmaps(
    VulkanContext&     ctx,
    vk::CommandBuffer& command_buffer,
    vk::Image          image,
    vk::Format         image_format,
    int32_t            texture_width,
    int32_t            texture_height,
    uint32_t           mip_levels
) {
    vk::ImageMemoryBarrier barrier = {
        .srcAccessMask       = vk::AccessFlagBits::eTransferWrite,
        .dstAccessMask       = vk::AccessFlagBits::eTransferRead,
        .oldLayout           = vk::ImageLayout::eTransferDstOptimal,
        .newLayout           = vk::ImageLayout::eTransferSrcOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange    = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .levelCount = 1,
                                 .layerCount = 1 }
    };

    vk::FormatProperties format_properties =
        ctx.m_physical_device.getFormatProperties(image_format);
    // we could implement a function that searches common image format for one
    // that does support blitting or implement mipmap generations using a
    // library like stb_image_resize
    if (!(format_properties.optimalTilingFeatures &
          vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
        throw std::runtime_error(
            "texture image format does not support linear blitting!"
        );
    }

    int32_t mip_width  = texture_width;
    int32_t mip_height = texture_width;
    for (uint32_t i = 1; i < mip_levels; i++) {

        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        command_buffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            {},
            {},
            {},
            barrier
        );

        vk::ImageBlit blit = {
            .srcSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                .mipLevel   = i - 1,
                                .layerCount = 1

            },
            .srcOffsets =
                std::array<vk::Offset3D, 2>({ {},
                                              { mip_width, mip_height, 1 } }),
            .dstSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                .mipLevel   = i,
                                .layerCount = 1 },

            .dstOffsets =
                std::array<vk::Offset3D, 2>{
                    { {},
                      {
                          1 < mip_width ? mip_width / 2 : 1,
                          1 < mip_height ? mip_height / 2 : 1,
                          1,
                      } } },
        };

        command_buffer.blitImage(
            image,
            vk::ImageLayout::eTransferSrcOptimal,
            image,
            vk::ImageLayout::eTransferDstOptimal,
            blit,
            vk::Filter::eLinear
        );

        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        command_buffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            {},
            {},
            {},
            barrier
        );

        if (1 < mip_width) {
            mip_width /= 2;
        }

        if (1 < mip_height) {
            mip_height /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
    barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    command_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        {},
        {},
        barrier
    );
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

    /*
     * We calculate how many times the image can be subdivided using log2
     * We floor to prevent cases where image is not a power of 2
     * we add 1 for base case, The image we have at the start is always a valid
     * level
     */
    ctx.m_mip_levels =
        static_cast<uint32_t>(
            std::floor(std::log2(std::max(texture_width, texture_height)))
        ) +
        1;

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
        ctx.m_mip_levels,
        vk::SampleCountFlagBits::e1,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferSrc |
            vk::ImageUsageFlagBits::eTransferDst |
            vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    // copying texture  image to gpu
    vk::CommandBuffer temp_cmd_buffer = begin_single_time_commands(ctx);
    transition_image_layout(
        temp_cmd_buffer,
        ctx.m_texture_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        ctx.m_mip_levels
    );
    copy_buffer_to_image(
        temp_cmd_buffer,
        staging_buffer,
        ctx.m_texture_image,
        static_cast<uint32_t>(texture_width),
        static_cast<uint32_t>(texture_height)
    );

    // We are doing this inside generate_mipmaps
    // transition_image_layout(
    //     temp_cmd_buffer,
    //     ctx.m_texture_image,
    //     vk::ImageLayout::eTransferDstOptimal,
    //     vk::ImageLayout::eShaderReadOnlyOptimal,
    //     ctx.m_mip_levels
    // );

    generate_mipmaps(
        ctx,
        temp_cmd_buffer,
        ctx.m_texture_image,
        vk::Format::eR8G8B8A8Srgb,
        texture_width,
        texture_height,
        ctx.m_mip_levels
    );
    end_single_time_commands(ctx, temp_cmd_buffer);

    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);
}

void transition_image_layout(
    vk::CommandBuffer command_buffer,
    vk::Image         image,
    vk::ImageLayout   old_layout,
    vk::ImageLayout   new_layout,
    uint32_t          mip_levels
) {
    vk::ImageMemoryBarrier barrier{

        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange    = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .levelCount = mip_levels,
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
    vk::ImageAspectFlags aspect_flag,
    uint32_t             mip_levels
) {
    vk::ImageViewCreateInfo view_info{

        .image            = image,
        .viewType         = vk::ImageViewType::e2D,
        .format           = format,
        .subresourceRange = { .aspectMask     = aspect_flag,
                              .baseMipLevel   = 0,
                              .levelCount     = mip_levels,
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
        vk::ImageAspectFlagBits::eColor,
        ctx.m_mip_levels
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
        .mipLodBias       = 0.0f,
        .anisotropyEnable = vk::True,
        .maxAnisotropy    = properties.limits.maxSamplerAnisotropy,
        .compareEnable    = vk::False,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 0.0f,
        .maxLod           = vk::LodClampNone
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
        1,
        ctx.m_msaa_samples,
        depth_format,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ctx.m_depth_image_view = create_image_view(
        ctx, ctx.m_depth_image, depth_format, vk::ImageAspectFlagBits::eDepth, 1
    );
    ctx.m_depth_format = depth_format;
}


void cleanup_color_resources(VulkanContext& ctx) {
    if (ctx.m_color_image_memory) {
        ctx.m_device.freeMemory(ctx.m_color_image_memory);
        ctx.m_color_image_memory = nullptr;
    }
    if (ctx.m_color_image) {
        ctx.m_device.destroyImage(ctx.m_color_image);
        ctx.m_color_image = nullptr;
    }
    if (ctx.m_color_image_view) {
        ctx.m_device.destroyImageView(ctx.m_color_image_view);
        ctx.m_color_image_view = nullptr;
    }
}

void create_color_resources(VulkanContext& ctx) {
    vk::Format color_format = ctx.m_swap_chain_surface_format.format;

    std::tie(ctx.m_color_image, ctx.m_color_image_memory) = create_image(
        ctx,
        ctx.m_swap_chain_extent.width,
        ctx.m_swap_chain_extent.height,
        1,
        ctx.m_msaa_samples,
        color_format,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransientAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ctx.m_color_image_view = create_image_view(
        ctx, ctx.m_color_image, color_format, vk::ImageAspectFlagBits::eColor, 1
    );
}

} // namespace varicle::render::vulkan

#include "renderer/vulkan/graphics/buffer.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include "renderer/vulkan/util/command.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <span>
#include <utility>

namespace varicle::renderer::vulkan {

void create_command_pool(VulkanContext& ctx) {
    vk::CommandPoolCreateInfo poolInfo{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = ctx.m_indices.graphics_family.value()
    };

    ctx.m_command_pool =
        vk::CommandPool(ctx.m_device.createCommandPool(poolInfo));

    // for Transfer
    vk::CommandPoolCreateInfo transfer_pool_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = ctx.m_indices.transfer_family.value()
    };

    ctx.m_transfer_command_pool =
        vk::CommandPool(ctx.m_device.createCommandPool(transfer_pool_info));
}

uint32_t find_memory_type(
    VulkanContext&          ctx,
    uint32_t                typeFilter,
    vk::MemoryPropertyFlags properties
) {
    vk::PhysicalDeviceMemoryProperties memProperties =
        ctx.m_physical_device.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

[[nodiscard(
    "Buffer needs to be freed manually"
)]] std::pair<vk::Buffer, vk::DeviceMemory>
create_buffer(
    VulkanContext&          ctx,
    vk::DeviceSize          size,
    vk::BufferUsageFlags    usage,
    vk::MemoryPropertyFlags properties,
    std::span<uint32_t>     queue_family
) {

    bool is_concurrent = queue_family.size() > 1;

    vk::BufferCreateInfo buffer_info{
        .size        = size,
        .usage       = usage,
        .sharingMode = is_concurrent ? vk::SharingMode::eConcurrent
                                     : vk::SharingMode::eExclusive,
        .queueFamilyIndexCount =
            is_concurrent ? static_cast<uint32_t>(queue_family.size()) : 0,
        .pQueueFamilyIndices = is_concurrent ? queue_family.data() : nullptr,
    };

    vk::Buffer             buffer = ctx.m_device.createBuffer(buffer_info);
    vk::MemoryRequirements mem_requirements =
        ctx.m_device.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo memory_allocate_info{
        .allocationSize = mem_requirements.size,
        .memoryTypeIndex =
            find_memory_type(ctx, mem_requirements.memoryTypeBits, properties)
    };
    auto buffer_memory = ctx.m_device.allocateMemory(memory_allocate_info);
    ctx.m_device.bindBufferMemory(buffer, buffer_memory, 0);
    return { buffer, buffer_memory };
}

void copy_buffer(
    VulkanContext& ctx,
    vk::Buffer&    src_buffer,
    vk::Buffer&    dst_buffer,
    vk::DeviceSize size
) {
    auto temp_command_copy_buffer = begin_single_time_commands(ctx);
    temp_command_copy_buffer.copyBuffer(
        src_buffer, dst_buffer, vk::BufferCopy(0, 0, size)
    );
    end_single_time_commands(ctx, temp_command_copy_buffer);
}

void create_command_buffers(VulkanContext& ctx) {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool        = ctx.m_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };
    ctx.m_command_buffers = ctx.m_device.allocateCommandBuffers(allocInfo);

    // for create one for buffer transer
    vk::CommandBufferAllocateInfo trasfer_alloc_info{
        .commandPool        = ctx.m_transfer_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };
    ctx.m_transfer_command_buffers =
        ctx.m_device.allocateCommandBuffers(trasfer_alloc_info);
}

void create_sync_objects(VulkanContext& ctx) {

    assert(
        ctx.m_present_complete_semaphores.empty() &&
        ctx.m_render_finished_semaphores.empty() &&
        ctx.m_in_flight_fences.empty()
    );
    for (size_t i = 0; i < ctx.m_swap_chain_images.size(); i++) {
        ctx.m_render_finished_semaphores.push_back(
            ctx.m_device.createSemaphore(vk::SemaphoreCreateInfo())
        );
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        ctx.m_present_complete_semaphores.push_back(
            ctx.m_device.createSemaphore(vk::SemaphoreCreateInfo())
        );

        ctx.m_in_flight_fences.push_back(ctx.m_device.createFence(
            vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }
        ));
    }
}

} // namespace varicle::renderer::vulkan

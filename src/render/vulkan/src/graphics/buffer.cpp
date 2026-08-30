#include "graphics/buffer.hpp"
#include "core/config.hpp"
#include "core/vertex.hpp"

namespace varicle::render::vulkan {

void create_command_pool(VulkanContext& ctx) {
    vk::CommandPoolCreateInfo poolInfo{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = ctx.m_indices.graphics_family.value()
    };

    ctx.m_command_pool =
        vk::CommandPool(ctx.m_device.createCommandPool(poolInfo));
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

void create_vertex_buffer(VulkanContext& ctx) {
    vk::BufferCreateInfo buffer_info{
        .size        = sizeof(vertices[0]) * vertices.size(),
        .usage       = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive
    };
    ctx.m_vertex_buffer = ctx.m_device.createBuffer(buffer_info);

    vk::MemoryRequirements memRequirements =
        ctx.m_device.getBufferMemoryRequirements(ctx.m_vertex_buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo{
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = find_memory_type(
            ctx,
            memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
        )
    };

    ctx.m_vertex_buffer_memory =
        ctx.m_device.allocateMemory(memoryAllocateInfo);
    ctx.m_device.bindBufferMemory(
        ctx.m_vertex_buffer, ctx.m_vertex_buffer_memory, 0
    );

    void* data =
        ctx.m_device.mapMemory(ctx.m_vertex_buffer_memory, 0, buffer_info.size);
    memcpy(data, vertices.data(), buffer_info.size);
    ctx.m_device.unmapMemory(ctx.m_vertex_buffer_memory);
}

void create_command_buffers(VulkanContext& ctx) {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool        = ctx.m_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };
    auto result           = ctx.m_device.allocateCommandBuffers(allocInfo);
    ctx.m_command_buffers = std::move(result);
}

void create_sync_objects(VulkanContext& ctx) {

    assert(
        ctx.m_present_complete_semaphores.empty() &&
        ctx.m_render_finished_semaphores.empty() && ctx.m_in_flight_fences.empty()
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

} // namespace varicle::render::vulkan

#pragma once
#include "core/context.hpp"
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {

inline vk::CommandBuffer begin_single_time_commands(VulkanContext& ctx) {
    vk::CommandBufferAllocateInfo alloc_info{
        .commandPool        = ctx.m_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    vk::CommandBuffer command_buffer =
        ctx.m_device.allocateCommandBuffers(alloc_info).front();
    vk::CommandBufferBeginInfo begin_info{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };
    command_buffer.begin(begin_info);
    return command_buffer;
}

// Note this will end and deallocate the command buffer
// This also blocks till gpu process command do not use in loop as there is no
// means of Synchronization between the gpu and cpu
inline void
end_single_time_commands(VulkanContext& ctx, vk::CommandBuffer command_buffer) {
    command_buffer.end();
    vk::SubmitInfo submit_info{ .commandBufferCount = 1,
                                .pCommandBuffers    = &command_buffer };
    ctx.m_graphics_queue.submit(submit_info, nullptr);
    ctx.m_graphics_queue.waitIdle();

    // free cmd buffer manually
    ctx.m_device.freeCommandBuffers(ctx.m_command_pool, command_buffer);
}

} // namespace varicle::render::vulkan

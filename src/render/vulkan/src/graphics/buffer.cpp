#include "graphics/buffer.hpp"
#include "core/config.hpp"
#include "graphics/resource.hpp"
#include "util/command.hpp"
#include <chrono>
#include <span>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Depth 0 - 1 rather than -1 to 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

namespace varicle::render::vulkan {

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

void create_uniform_buffer(VulkanContext& ctx) {

    std::array<uint32_t, 1> qf{ ctx.m_indices.graphics_family.value() };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DeviceSize buffer_size   = sizeof(UniformBufferObject);
        auto [buffer, buffer_memory] = create_buffer(
            ctx,
            buffer_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent,
            qf
        );
        ctx.m_uniform_buffers.push_back(buffer);
        ctx.m_uniform_buffers_memory.push_back(buffer_memory);
        ctx.m_uniform_buffers_mapped.push_back(ctx.m_device.mapMemory(
            ctx.m_uniform_buffers_memory.back(), 0, buffer_size
        ));
    }
}
void update_uniform_buffer(VulkanContext& ctx) {
    static auto start_time   = std::chrono::high_resolution_clock::now();
    auto        current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     current_time - start_time
    )
                     .count();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(
        glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    );
    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(ctx.m_swap_chain_extent.width) /
            static_cast<float>(ctx.m_swap_chain_extent.height),
        0.1f,
        10.0f
    );
    ubo.proj[1][1] *= -1; // flip y

    // This is not that efficient. look into push constants
    memcpy(ctx.m_uniform_buffers_mapped[ctx.m_frame_index], &ubo, sizeof(ubo));
}

void create_index_buffer(VulkanContext& ctx, Mesh& mesh) {

    // Size of bfufer
    vk::DeviceSize buffer_size =
        sizeof(mesh.m_indices[0]) * mesh.m_indices.size();

    // graphics_family incase we transfer
    std::array<uint32_t, 2> qf = { ctx.m_indices.graphics_family.value(),
                                   ctx.m_indices.transfer_family.value() };

    // staging buffer for host visible buffer
    auto [staging_buffer, staging_buffer_memory] = create_buffer(
        ctx,
        buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        qf
    );

    void* data = ctx.m_device.mapMemory(staging_buffer_memory, 0,
    buffer_size); memcpy(data, mesh.m_indices.data(),
    static_cast<size_t>(buffer_size));
    ctx.m_device.unmapMemory(staging_buffer_memory);

    std::tie(mesh.m_index_buffer, mesh.m_index_buffer_memory) = create_buffer(
        ctx,
        buffer_size,
        vk::BufferUsageFlagBits::eIndexBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        qf
    );

    copy_buffer(ctx, staging_buffer, mesh.m_index_buffer, buffer_size);

    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);

}

void create_vertex_buffer(VulkanContext& ctx, Mesh& mesh) {
    // We get the size of the buffer we want to create
    vk::DeviceSize buffer_size =
        sizeof(mesh.m_vertices[0]) * mesh.m_vertices.size();

    // we just now
    std::array<uint32_t, 2> qf = { ctx.m_indices.graphics_family.value(),
                                   ctx.m_indices.transfer_family.value() };

    // create a host visible buffer for staging
    auto [staging_buffer, staging_buffer_memory] = create_buffer(
        ctx,
        buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        qf
    );

    // Map the data and transfer the data from our vector to the staging
    // buffer
    void* data = ctx.m_device.mapMemory(staging_buffer_memory, 0, buffer_size);
    memcpy(data, mesh.m_vertices.data(), buffer_size);
    ctx.m_device.unmapMemory(staging_buffer_memory);

    // Here we create the gpu local buffer
    std::tie(mesh.m_vertex_buffer, mesh.m_vertex_buffer_memory) = create_buffer(
        ctx,
        buffer_size,
        vk::BufferUsageFlagBits::eVertexBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        qf
    );

    copy_buffer(ctx, staging_buffer, mesh.m_vertex_buffer, buffer_size);

    // finally we release the staging buffer
    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);
}

// void create_index_buffer(VulkanContext& ctx) {
//     vk::DeviceSize buffer_size = sizeof(ctx.indices[0]) * ctx.indices.size();
//
//     std::array<uint32_t, 2> qf = { ctx.m_indices.graphics_family.value(),
//                                    ctx.m_indices.transfer_family.value() };
//
//     auto [staging_buffer, staging_buffer_memory] = create_buffer(
//         ctx,
//         buffer_size,
//         vk::BufferUsageFlagBits::eTransferSrc,
//         vk::MemoryPropertyFlagBits::eHostVisible |
//             vk::MemoryPropertyFlagBits::eHostCoherent,
//         qf
//     );
//
//     void* data = ctx.m_device.mapMemory(staging_buffer_memory, 0,
//     buffer_size); memcpy(data, ctx.indices.data(),
//     static_cast<size_t>(buffer_size));
//     ctx.m_device.unmapMemory(staging_buffer_memory);
//     std::tie(ctx.m_index_buffer, ctx.m_index_buffer_memory) = create_buffer(
//         ctx,
//         buffer_size,
//         vk::BufferUsageFlagBits::eIndexBuffer |
//             vk::BufferUsageFlagBits::eTransferDst,
//         vk::MemoryPropertyFlagBits::eDeviceLocal,
//         qf
//     );
//
//     copy_buffer(ctx, staging_buffer, ctx.m_index_buffer, buffer_size);
//     ctx.m_device.freeMemory(staging_buffer_memory);
//     ctx.m_device.destroyBuffer(staging_buffer);
// }

// void create_vertex_buffer(VulkanContext& ctx) {
//     vk::DeviceSize          buffer_size = sizeof(ctx.vertices[0]) *
//     ctx.vertices.size(); std::array<uint32_t, 2> qf = {
//     ctx.m_indices.graphics_family.value(),
//                                    ctx.m_indices.transfer_family.value() };
//
//     auto [staging_buffer, staging_buffer_memory] = create_buffer(
//         ctx,
//         buffer_size,
//         vk::BufferUsageFlagBits::eTransferSrc,
//         vk::MemoryPropertyFlagBits::eHostVisible |
//             vk::MemoryPropertyFlagBits::eHostCoherent,
//         qf
//     );
//     void* data = ctx.m_device.mapMemory(staging_buffer_memory, 0,
//     buffer_size); memcpy(data, ctx.vertices.data(), buffer_size);
//     ctx.m_device.unmapMemory(staging_buffer_memory);
//
//     std::tie(ctx.m_vertex_buffer, ctx.m_vertex_buffer_memory) =
//     create_buffer(
//         ctx,
//         buffer_size,
//         vk::BufferUsageFlagBits::eVertexBuffer |
//             vk::BufferUsageFlagBits::eTransferDst,
//         vk::MemoryPropertyFlagBits::eDeviceLocal,
//         qf
//     );
//
//     copy_buffer(ctx, staging_buffer, ctx.m_vertex_buffer, buffer_size);
//     ctx.m_device.freeMemory(staging_buffer_memory);
//     ctx.m_device.destroyBuffer(staging_buffer);
// }

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

} // namespace varicle::render::vulkan

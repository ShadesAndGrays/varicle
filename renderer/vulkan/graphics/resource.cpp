#include "renderer/vulkan/graphics/resource.hpp"
#include "renderer/vulkan/graphics/buffer.hpp"

using namespace varicle::renderer::vulkan;

void MeshData::create_index_buffer(VulkanContext& ctx, MeshData& mesh) {

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

    void* data = ctx.m_device.mapMemory(staging_buffer_memory, 0, buffer_size);
    memcpy(data, mesh.m_indices.data(), static_cast<size_t>(buffer_size));
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

void MeshData::create_vertex_buffer(VulkanContext& ctx, MeshData& mesh) {
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

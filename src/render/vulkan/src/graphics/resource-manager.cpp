#include "resource-manager.hpp"
#include "core/context.hpp"
#include "graphics/buffer.hpp"
#include "graphics/image.hpp"
#include "resource.hpp"
#include "util/command.hpp"

#include <print>
#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace varicle::render::vulkan {

Texture& ResourceManager::get_texture(
    vulkan::VulkanContext& ctx,
    TextureHandle          texture
) {
    return m_textures[texture];
}
Texture ResourceManager::create_default_texture(VulkanContext& ctx) {
    Texture texture;

    int texture_width, texture_height, texture_channels;

    std::array<unsigned char, 4> pixels = { 255, 255, 255, 255 };
    texture_width                       = 1;
    texture_height                      = 1;
    texture_channels                    = 4;

    texture.m_mip_levels =
        static_cast<uint32_t>(
            std::floor(std::log2(std::max(texture_width, texture_height)))
        ) +
        1;

    vk::DeviceSize image_buffer_size =
        texture_width * texture_height * 4; // 4 channels rgba;

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
    memcpy(data, pixels.data(), image_buffer_size);
    // stbi_image_free(pixels);

    std::tie(texture.m_image, texture.m_image_memory) = create_image(
        ctx,
        texture_width,
        texture_height,
        texture.m_mip_levels,
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
        texture.m_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        texture.m_mip_levels
    );
    copy_buffer_to_image(
        temp_cmd_buffer,
        staging_buffer,
        texture.m_image,
        static_cast<uint32_t>(texture_width),
        static_cast<uint32_t>(texture_height)
    );

    generate_mipmaps(
        ctx,
        temp_cmd_buffer,
        texture.m_image,
        vk::Format::eR8G8B8A8Srgb,
        texture_width,
        texture_height,
        texture.m_mip_levels
    );
    end_single_time_commands(ctx, temp_cmd_buffer);

    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);

    return std::move(texture);
}

TextureHandle ResourceManager::load_texture(
    vulkan::VulkanContext& ctx,
    const char*            filepath
) {

    Texture texture = load_texture_data(ctx, filepath);
    return add_texture(ctx, std::move(texture));
}

TextureHandle
ResourceManager::add_texture(VulkanContext& ctx, Texture&& texture) {
    m_textures.push_back(std::move(texture));
    return static_cast<MeshHandle>(m_textures.size() - 1);
}

void ResourceManager::unload_texture(
    vulkan::VulkanContext& ctx,
    TextureHandle          texture
) {}

MeshHandle

ResourceManager::load_mesh(vulkan::VulkanContext& ctx, const char* filepath) {

    Mesh mesh = load_mesh_data(ctx, filepath);
    create_vertex_buffer(ctx,mesh);
    create_index_buffer(ctx,mesh);
    return add_mesh(ctx, std::move(mesh));
}
MeshHandle ResourceManager::add_mesh(vulkan::VulkanContext& ctx, Mesh&& mesh) {
    m_meshes.push_back(std::move(mesh));
    return static_cast<MeshHandle>(m_meshes.size() - 1);
}

Mesh& ResourceManager::get_mesh(vulkan::VulkanContext& ctx, MeshHandle mesh) {
    return m_meshes[mesh];
}

void ResourceManager::unload_mesh(vulkan::VulkanContext& ctx, MeshHandle mesh) {
    Mesh mesh_to_unload = std::move(m_meshes[mesh]);
    unload_mesh(ctx, std::move(mesh_to_unload));
}

Mesh ResourceManager::load_mesh_data(
    VulkanContext& ctx,
    const char*    model_path
) {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      warn, err;

    if (!tinyobj::LoadObj(
            &attrib, &shapes, &materials, &warn, &err, model_path
        )) {
        throw std::runtime_error(warn + err);
    }

    Mesh mesh;

    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };

            vertex.tex_coord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };

            vertex.color = {
                1.0f,
                1.0f,
                1.0f,
            };

            auto [it, inserted] = unique_vertices.insert(
                { vertex, static_cast<uint32_t>(mesh.m_vertices.size()) }
            );
            if (inserted) {
                mesh.m_vertices.push_back(vertex);
            }
            mesh.m_indices.push_back(it->second);
        }
    }
    return mesh;
}

Texture
ResourceManager::load_texture_data(VulkanContext& ctx, const char* path) {
    Texture texture;
    if (ctx.m_command_pool == nullptr) {
        throw std::runtime_error(
            "Can't create image without a graphics-enabled command pool"
        );
    }

    int texture_width, texture_height, texture_channels;

    stbi_uc* pixels = stbi_load(
        path, &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha
    );

    /*
     * We calculate how many times the image can be subdivided using log2
     * We floor to prevent cases where image is not a power of 2
     * we add 1 for base case, The image we have at the start is always a valid
     * level
     */
    texture.m_mip_levels =
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

    std::tie(texture.m_image, texture.m_image_memory) = create_image(
        ctx,
        texture_width,
        texture_height,
        texture.m_mip_levels,
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
        texture.m_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        texture.m_mip_levels
    );
    copy_buffer_to_image(
        temp_cmd_buffer,
        staging_buffer,
        texture.m_image,
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
        texture.m_image,
        vk::Format::eR8G8B8A8Srgb,
        texture_width,
        texture_height,
        texture.m_mip_levels
    );
    end_single_time_commands(ctx, temp_cmd_buffer);

    ctx.m_device.freeMemory(staging_buffer_memory);
    ctx.m_device.destroyBuffer(staging_buffer);

    texture.m_image_view = create_image_view(
        ctx,
        texture.m_image,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        texture.m_mip_levels
    );

    return texture;
}

void ResourceManager::unload_texture(VulkanContext& ctx, Texture texture) {

    ctx.m_device.free(texture.m_image_memory);
    ctx.m_device.destroyImageView(texture.m_image_view);
    ctx.m_device.destroyImage(texture.m_image);

}

void ResourceManager::unload_mesh(VulkanContext& ctx, Mesh mesh) {
    ctx.m_device.free(mesh.m_index_buffer_memory);
    ctx.m_device.free(mesh.m_vertex_buffer_memory);
    ctx.m_device.destroyBuffer(mesh.m_vertex_buffer);
    ctx.m_device.destroyBuffer(mesh.m_index_buffer);
    mesh.m_indices.clear();
    mesh.m_vertices.clear();
}

void ResourceManager::unload_all_texures(VulkanContext& ctx) {
    static int num = 0;
    for (auto& i : m_textures) {
        num++;
        unload_texture(ctx, std::move(i));
    }
    std::println("Unloaded {} textures", num);
}

void ResourceManager::unload_all_meshes(VulkanContext& ctx) {
    static int num = 0;
    for (auto& i : m_meshes) {
        num++;
        unload_mesh(ctx, std::move(i));
    }

    std::println("Unloaded {} meshes", num);
}

} // namespace varicle::render::vulkan

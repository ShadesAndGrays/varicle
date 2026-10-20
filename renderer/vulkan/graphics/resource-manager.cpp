#include "renderer/vulkan/graphics/resource-manager.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/buffer.hpp"
#include "renderer/vulkan/graphics/image.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include "renderer/vulkan/util/command.hpp"

#include "renderer/vulkan/util/slang-compiler.hpp"
#include "vendor/stb/stb_image.h"
#include "vendor/tinyobjloader/tiny_obj_loader.h"
#include <print>
#include <unordered_map>

namespace varicle::renderer::vulkan {

TextureData& ResourceManager::get_texture(TextureHandle texture) {
    return m_textures[texture];
}
TextureData ResourceManager::create_default_texture(VulkanContext& ctx) {
    TextureData texture;

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

    texture.m_image_view = create_image_view(
        ctx,
        texture.m_image,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        texture.m_mip_levels
    );

    texture.m_descriptor_image_info = {
        .sampler     = ctx.m_texture_sampler,
        .imageView   = texture.m_image_view,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
    };

    return std::move(texture);
}

TextureHandle ResourceManager::load_texture(
    vulkan::VulkanContext& ctx,
    const char*            filepath
) {

    TextureData texture = load_texture_data(ctx, filepath);
    return add_texture(std::move(texture));
}

TextureHandle ResourceManager::add_texture(TextureData&& texture) {
    return m_textures.push(std::move(texture));
}

void ResourceManager::unload_texture(vk::Device device, TextureHandle texture) {
    throw std::runtime_error("Not implemented");
}

MeshHandle

ResourceManager::load_mesh(vulkan::VulkanContext& ctx, const char* filepath) {
    MeshData mesh = load_mesh_data(filepath);
    MeshData::create_vertex_buffer(ctx, mesh);
    MeshData::create_index_buffer(ctx, mesh);
    return add_mesh(std::move(mesh));
}
MeshHandle ResourceManager::add_mesh(MeshData&& mesh) {
    return m_meshes.push(std::move(mesh));
}

void ResourceManager::load_primitives(vulkan::VulkanContext& ctx) {

    constexpr PrimitiveType all_primitives[] = {
        PrimitiveType::TRIANGLE,
        PrimitiveType::QUAD,
        PrimitiveType::TWO_QUAD_CUBE,
        PrimitiveType::FULLSCREEN_TRIANGLE,
    };

    for (uint32_t i = 0; i < static_cast<uint32_t>(PrimitiveType::LAST); i++) {

        auto primitive =
            varicle::renderer::get_primitive(static_cast<PrimitiveType>(i));

        println("loaded primitive {}", i);
        MeshData mesh = {
            primitive.verticies,
            primitive.indices,
        };

        MeshData::create_index_buffer(ctx, mesh);
        MeshData::create_vertex_buffer(ctx, mesh);

        m_primitives[i] = add_mesh(std::move(mesh));
    }
}
MeshData& ResourceManager::get_primitive(std::uint32_t idx) {
    return m_meshes[m_primitives[idx]];
}

MeshData& ResourceManager::get_mesh(MeshHandle mesh) {
    return m_meshes[mesh];
}

void ResourceManager::unload_mesh(vk::Device device, MeshHandle mesh) {
    MeshData mesh_to_unload = std::move(m_meshes[mesh]);
    unload_mesh_data(device, std::move(mesh_to_unload));
}

MeshData ResourceManager::load_mesh_data(const char* model_path) {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, model_path)) {
        throw std::runtime_error(err);
    }

    MeshData mesh;

    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };

            vertex.uv = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };

            vertex.normal = {
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

TextureData
ResourceManager::load_texture_data(VulkanContext& ctx, const char* path) {
    TextureData texture;
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

    texture.m_descriptor_image_info = {
        .sampler     = ctx.m_texture_sampler,
        .imageView   = texture.m_image_view,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
    };

    return texture;
}

void ResourceManager::unload_texture_data(
    vk::Device  device,
    TextureData texture
) {

    device.free(texture.m_image_memory);
    device.destroyImageView(texture.m_image_view);
    device.destroyImage(texture.m_image);
}

void ResourceManager::unload_mesh_data(vk::Device device, MeshData mesh) {
    device.free(mesh.m_index_buffer_memory);
    device.free(mesh.m_vertex_buffer_memory);
    device.destroyBuffer(mesh.m_vertex_buffer);
    device.destroyBuffer(mesh.m_index_buffer);
    mesh.m_indices.clear();
    mesh.m_vertices.clear();
}

void ResourceManager::unload_all_texures(vk::Device device) {
    static int num = 0;
    for (auto& i : m_textures.get_data()) {
        num++;
        unload_texture_data(device, std::move(i));
    }
    std::println("Unloaded {} textures", num);
}

void ResourceManager::unload_all_meshes(vk::Device device) {
    static int num = 0;
    for (auto& i : m_meshes.get_data()) {
        num++;
        unload_mesh_data(device, std::move(i));
    }

    std::println("Unloaded {} meshes", num);
}

MaterialData& ResourceManager::get_material(MaterialHandle material) {
    return m_materials[material];
}

MaterialHandle ResourceManager::add_material(
    vulkan::VulkanContext& ctx,
    MaterialData&&         material
) {
    return m_materials.push(std::move(material));
}

// ResourceManager::
//  MaterialHandle create_material(VulkanContext& ctx) {
//
//      }

void ResourceManager::unload_material_data(
    VulkanContext&    ctx,
    DescriptorSystem& ds,
    MaterialData      material
) {
    ds.pool_allocator.deallocate(
        ctx.m_device, material.m_descriptor_set, material.m_descriptor_pool_idx
    );
}

void ResourceManager::unload_all_materials(
    VulkanContext&    ctx,
    DescriptorSystem& ds
) {

    static int num = 0;
    for (auto& i : m_materials.get_data()) {
        num++;
        unload_material_data(ctx, ds, std::move(i));
    }
    ds.pool_allocator.reset(ctx.m_device);

    std::println("Unloaded {} materials", num);
}
[[nodiscard]]
std::vector<ShaderHandle> ResourceManager::load_shader(
    VulkanContext&                 ctx,
    const std::string              path,
    const std::vector<std::string> entry_points,
    const std::vector<std::string> include_dirs
) {

    std::vector<ShaderHandle> shader_handles;

    auto compiled_shaders =
        compile_shader_code(path, entry_points, include_dirs);

    for (auto& compiled_shader : compiled_shaders) {
        auto shader_code = compiled_shader.code;
        shader_handles.push_back(
            m_shaders.push(create_shader_module(ctx, shader_code))
        );
    }

    return shader_handles;
}
void ResourceManager::unload_shader(vk::Device device, ShaderHandle shader) {
    auto& shader_module = get_shader(shader);
    unload_shader_data(device, shader_module);
    m_shaders.remove(shader);
}
void ResourceManager::unload_shader_data(
    vk::Device       device,
    vk::ShaderModule shader_module
) {
    device.destroyShaderModule(shader_module);
}

void ResourceManager::unload_all_shaders(vk::Device device) {

    static int num = 0;
    for (auto& i : m_shaders.get_data()) {
        num++;
        unload_shader_data(device, i);
    }
    m_shaders.clear();

    std::println("Unloaded {} shaders", num);
};

vk::ShaderModule& ResourceManager::get_shader(ShaderHandle shader) {
    return m_shaders[shader];
}

} // namespace varicle::renderer::vulkan

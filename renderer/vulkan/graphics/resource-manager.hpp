#pragma once

#include "core/slot-map.hpp"
#include "renderer/common/handles.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include "vulkan/vulkan.hpp"
#include <array>

namespace varicle::renderer::vulkan {

class ResourceManager {
    // Just store all the primitives for now
    std::array<MeshHandle, 4> m_primitives = {};

    structures::SlotMap<TextureData>      m_textures;
    structures::SlotMap<MeshData>         m_meshes;
    structures::SlotMap<MaterialData>     m_materials;
    structures::SlotMap<vk::ShaderModule> m_shaders;

  public:
    // Texture
    TextureData&  get_texture(TextureHandle texture);
    TextureHandle add_texture(TextureData&& texture);
    TextureHandle load_texture(VulkanContext& ctx, const char* filepath);

    [[nodiscard]] TextureData create_default_texture(VulkanContext& ctx);
    void unload_texture(vk::Device device, TextureHandle texture);
    void unload_all_texures(vk::Device device);

    // Meshes
    MeshData&  get_mesh(MeshHandle mesh);
    MeshHandle add_mesh(MeshData&& mesh);
    MeshHandle load_mesh(VulkanContext& ctx, const char* filepath);
    void       unload_mesh(vk::Device device, MeshHandle mesh);
    void       unload_all_meshes(vk::Device device);
    void       load_primitives(VulkanContext& ctx);
    MeshData&  get_primitive(std::uint32_t idx);

    // Material
    MaterialData& get_material(MaterialHandle material);
    MaterialHandle
    add_material(vulkan::VulkanContext& ctx, MaterialData&& material);

    MaterialHandle create_material(VulkanContext& ctx);
    void           unload_material_data(
        VulkanContext&    ctx,
        DescriptorSystem& ds,
        MaterialData      material
    );
    void unload_all_materials(VulkanContext& ctx, DescriptorSystem& ds);

    // Shaders
    [[nodiscard]]
    vk::ShaderModule& get_shader(ShaderHandle shader);

    [[nodiscard]]
    std::vector<ShaderHandle> load_shader(
        VulkanContext&                 ctx,
        const std::string              path,
        const std::vector<std::string> entry_points,
        const std::vector<std::string> include_dirs
    );
    void unload_shader(vk::Device device, ShaderHandle shader);
    void unload_shader_data(vk::Device device, vk::ShaderModule shader_module);
    void unload_all_shaders(vk::Device device);

  private:
    TextureData load_texture_data(VulkanContext& ctx, const char* path);
    void        unload_texture_data(vk::Device device, TextureData texture);

    MeshData load_mesh_data(const char* path);
    void     unload_mesh_data(vk::Device device, MeshData mesh);
};

} // namespace varicle::renderer::vulkan

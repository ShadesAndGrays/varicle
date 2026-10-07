#pragma once

#include "renderer/common/handles.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include "vulkan/vulkan.hpp"

namespace varicle::renderer::vulkan {

class ResourceManager {

    std::vector<TextureData>  m_textures;
    std::vector<MeshData>     m_meshes;
    std::vector<MaterialData> m_materials;

  public:
    TextureData&  get_texture(TextureHandle texture);
    TextureHandle add_texture(TextureData&& texture);
    TextureHandle load_texture(VulkanContext& ctx, const char* filepath);

    [[nodiscard]] TextureData create_default_texture(VulkanContext& ctx);
    void unload_texture(vk::Device device, TextureHandle texture);
    void unload_all_texures(vk::Device device);

    MeshData&  get_mesh(MeshHandle mesh);
    MeshHandle add_mesh(MeshData&& mesh);
    MeshHandle load_mesh(VulkanContext& ctx, const char* filepath);
    void       unload_mesh(vk::Device device, MeshHandle mesh);
    void       unload_all_meshes(vk::Device device);

    MaterialData& get_material(MaterialHandle material);
    MaterialHandle
    add_material(vulkan::VulkanContext& ctx, MaterialData&& material);

    MaterialHandle create_material(VulkanContext& ctx);
    void           unload_material(VulkanContext& ctx, MaterialHandle material);
    void           unload_all_materials(VulkanContext& ctx);

  private:
    TextureData load_texture_data(VulkanContext& ctx, const char* path);
    void        unload_texture(vk::Device device, TextureData texture);

    MeshData load_mesh_data(const char* path);
    void     unload_mesh(vk::Device device, MeshData mesh);
};

} // namespace varicle::renderer::vulkan

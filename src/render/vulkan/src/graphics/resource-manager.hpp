#pragma once

#include "core/context.hpp"
#include "handles.hpp"
#include "resource.hpp"

namespace varicle::render::vulkan {

class ResourceManager {

  private:
    std::vector<Mesh>    m_meshes;
    std::vector<Texture> m_textures;

  public:
    Texture& get_texture(vulkan::VulkanContext& ctx, TextureHandle texture);
    [[nodiscard]] Texture create_default_texture(VulkanContext& ctx);
    TextureHandle         add_texture(VulkanContext& ctx, Texture&& texture);
    TextureHandle load_texture(VulkanContext& ctx, const char* filepath);
    void          unload_texture(VulkanContext& ctx, TextureHandle texture);
    void          unload_all_texures(VulkanContext& ctx);

    Mesh&      get_mesh(vulkan::VulkanContext& ctx, MeshHandle mesh);
    MeshHandle add_mesh(vulkan::VulkanContext& ctx, Mesh&& mesh);
    MeshHandle load_mesh(VulkanContext& ctx, const char* filepath);
    void       unload_mesh(VulkanContext& ctx, MeshHandle mesh);
    void       unload_all_meshes(VulkanContext& ctx);


    Material&      get_material(vulkan::VulkanContext& ctx, MaterialHandle material);
    MaterialHandle add_material(vulkan::VulkanContext& ctx, Material&& material);
    MaterialHandle load_material(VulkanContext& ctx, const char* filepath);
    void       unload_material(VulkanContext& ctx, MaterialHandle material);
    void       unload_all_materials(VulkanContext& ctx);
  private:
    Texture load_texture_data(VulkanContext& ctx, const char* path);
    void    unload_texture(VulkanContext& ctx, Texture texture);

    Mesh load_mesh_data(VulkanContext& ctx, const char* path);
    void unload_mesh(VulkanContext& ctx, Mesh mesh);
};

} // namespace varicle::render::vulkan

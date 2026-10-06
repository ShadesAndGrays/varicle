#pragma once

#include "renderer/common/color.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/pipelin2.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource-manager.hpp"
#include "renderer/vulkan/util/util.hpp"

namespace varicle::renderer::vulkan {

VulkanContext init(Window& window);
void          set_clear_color(VulkanContext& ctx, Color background);
bool          should_close(VulkanContext& ctx);
void          begin_frame(VulkanContext& ctx);
void          end_frame(VulkanContext& ctx);
void          draw_primitive(
    VulkanContext&   ctx,
    ResourceManager& rm,
    PrimitiveType    type
);
void cleanup(VulkanContext& ctx);

ResourceManager  create_resource_manager(VulkanContext& ctx);
DescriptorSystem create_descriptor_system(VulkanContext& ctx);

class VulkanRenderer {

  public:
    void init(
        uint32_t    width  = 800,
        uint32_t    height = 600,
        const char* title  = "Varicle"
    );

    // void shutdown();
    //
    // void begin_frame(bool clear_screen = true);
    // void set_clear_color(Color background);
    // void end_frame();
    //
    // void resize(uint32_t width, uint32_t height);
    // bool should_close_window();

    // TextureHandle load_texture(const char* filepath);
    // void          destroy_texture(TextureHandle texture);
    //
    // TextureHandle load_mesh(const char* filepath);
    // void          destroy_mesh(MeshHandle mesh);
    //
    // void draw_rect(const Rect& rect, const Color& color);
    //
    // void draw_object(Object object);
    //
    // void draw_texured_rect(
    //     const Rect&   rect,
    //     TextureHandle texture,
    //     const Color&  tint = { 1, 1, 1, 1 }
    // ) {}

    // void    set_camera(Camera camera);
    // Camera& get_camera();

    // float get_aspect() { return m_window.get_aspect(); }

    // private:
    // struct Impl;
    // ResourceManager m_resource_manager;
    // Impl*           impl = nullptr;
    // Camera          m_camera; // Default camera
    // Window          m_window;
};

} // namespace varicle::renderer::vulkan

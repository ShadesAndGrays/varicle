#pragma once

// IWYU pragma: begin_exports
#include "core/color.hpp"
#include "core/type.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/render-camera.hpp"
#include "renderer/vulkan/graphics/resource-manager.hpp"
#include "renderer/vulkan/util/util.hpp"
// IWYU pragma: end_exports

namespace varicle::renderer::vulkan {

VulkanContext init(Window& window);
void          set_clear_color(VulkanContext& ctx, varicle::Colorf background);
bool          should_close(VulkanContext& ctx);
void          begin_frame(VulkanContext& ctx);
void          end_frame(VulkanContext& ctx);
void          draw_primitive(
    VulkanContext&   ctx,
    ResourceManager& rm,
    PrimitiveType    type,
    MaterialHandle   material,
    const Transform3D& transform,
    RenderCamera&    camera
);

void cleanup(VulkanContext& ctx, DescriptorSystem& ds, ResourceManager& rm);

ResourceManager  create_resource_manager(VulkanContext& ctx);
DescriptorSystem create_descriptor_system(VulkanContext& ctx);

} // namespace varicle::renderer::vulkan

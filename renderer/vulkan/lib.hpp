#pragma once

#include "renderer/common/color.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"
#include "renderer/vulkan/util/util.hpp"
#include "renderer/vulkan/core/context.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource-manager.hpp"

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

} // namespace varicle::renderer::vulkan

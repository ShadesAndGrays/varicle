#include <core/context.hpp>

namespace varicle::render::vulkan {

void create_descriptor_set_layout(VulkanContext& ctx);
void create_descriptor_pool(VulkanContext& ctx);
void create_descriptor_set(VulkanContext& ctx);

void create_graphics_pipeline(VulkanContext& ctx);

[[nodiscard]] vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code);

} // namespace varicle::render::vulkan

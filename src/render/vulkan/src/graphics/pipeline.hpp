#include <core/context.hpp>

namespace varicle::render::vulkan {

void create_graphics_pipeline(VulkanContext& ctx);

[[nodiscard]] vk::ShaderModule
create_shader_module(const VulkanContext& ctx, const std::vector<char>& code);

} // namespace varicle::render::vulkan

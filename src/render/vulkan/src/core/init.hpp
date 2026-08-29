#include <vulkan/vulkan.hpp>

namespace varicle::render {

vk::ApplicationInfo create_app();

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*> validation_layers,
    vk::ApplicationInfo*           p_app_info,
    std::vector<const char*>       required_layers,
    std::vector<const char*>       required_extensions
);

vk::Instance create_instance();

std::vector<const char*> get_required_extensions();

} // namespace varicle::render

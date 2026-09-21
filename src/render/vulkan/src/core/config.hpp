#pragma once
#include <vector>

namespace varicle::render::vulkan {
#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

const std::vector<char const*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

} // namespace varicle::render::vulkan

#pragma once
#include "core/context.hpp"
#include "vulkan-renderer.hpp"
namespace varicle::render::vulkan {
struct VulkanRenderer::Impl {
    VulkanContext v_context;
};
} // namespace varicle::render::vulkan

#pragma once
#include "core/context.hpp"
#include <print>
#include <vulkan/vulkan.hpp>


namespace varicle::render::vulkan {
namespace debug {

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
    vk::DebugUtilsMessageTypeFlagsEXT             type,
    const vk::DebugUtilsMessengerCallbackDataEXT* pcallbackData,
    void*                                         pUserData
) {
    std::println(
        "validation layer: type: {}  msg: {}",
        to_string(type),
        pcallbackData->pMessage
    );
    return vk::False;
}

void setupDebugMessenger(VulkanContext& context);

} // namespace debug

std::vector<const char*> getRequiredInstanceExtenstions();

std::vector<const char*> get_required_layers();

std::vector<char const*> get_required_extensions();

} // namespace varicle::render::vulkan

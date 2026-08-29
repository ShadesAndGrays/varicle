#include <print>
#include <vulkan/vulkan.hpp>
#include "core/context.hpp"

namespace varicle::render::vulkan::debug {

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

void setupDebugMessenger(VulkanContext &context) {
    // Debug Messenger

    /*
     * For configuring layers check out the
     * vk_layer_settings.txt in the VKSDK
     *
     */

    std::println("Setting up Debug messenger");

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
        .messageSeverity = severityFlags,
        .messageType     = messageTypeFlags,
        .pfnUserCallback = &debugCallback
    };


    context.m_debugMessanger =
        context.m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo);
}

} // namespace varicle::render::vulkan

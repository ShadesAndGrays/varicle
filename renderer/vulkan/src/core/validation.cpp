#include "validation.hpp"
#include "GLFW/glfw3.h"
#include <algorithm>
#include "core/config.hpp"

void varicle::render::vulkan::debug::setupDebugMessenger(
    VulkanContext& ctx
) {
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

    ctx.m_debugMessenger = ctx.m_instance.createDebugUtilsMessengerEXT(
        debugUtilsMessengerCreateInfo
    );
}
std::vector<const char*>
varicle::render::vulkan::getRequiredInstanceExtenstions() {
    // All extension from glfw are required as we are using it for windowing
    uint32_t glfw_exetension_count = 0;
    auto     glfw_extensions =
        glfwGetRequiredInstanceExtensions(&glfw_exetension_count);
    std::vector extensions(
        glfw_extensions, glfw_extensions + glfw_exetension_count
    );

    // I want to enable callback debugging
    if (enable_validation_layers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}
std::vector<const char*> varicle::render::vulkan::get_required_layers() {

    std::vector<char const*> required_layers;
    if (enable_validation_layers) {
        required_layers.assign(
            validation_layers.begin(), validation_layers.end()
        );
    }

    auto layer_properties = vk::enumerateInstanceLayerProperties();

    auto unsupported_layer_it = std::ranges::find_if(
        required_layers, [&layer_properties](auto const& required_layer) {
            return std::ranges::none_of(
                layer_properties, [required_layer](auto const& layer_property) {
                    return strcmp(layer_property.layerName, required_layer) ==
                        0;
                }
            );
        }
    );

    if (unsupported_layer_it != required_layers.end()) {
        throw std::runtime_error(
            "Require layer not supported: " + std::string(*unsupported_layer_it)
        );
    }
    return required_layers;
}

std::vector<char const*> varicle::render::vulkan::get_required_extensions() {

    auto required_extensions = getRequiredInstanceExtenstions();

    // get instance extensions for this instance
    auto extension_properties = vk::enumerateInstanceExtensionProperties();

    auto unsupportede_property_it = std::ranges::find_if(
        required_extensions,
        [&extension_properties](auto const& required_extension) {
            return std::ranges::none_of(
                extension_properties,
                [required_extension](auto const& extension_property) {
                    return strcmp(
                               extension_property.extensionName,
                               required_extension
                           ) == 0;
                }
            );
        }

    );

    if (unsupportede_property_it != required_extensions.end()) {
        throw std::runtime_error(
            "Require extension not supported: " +
            std::string(*unsupportede_property_it)
        );
    }

    return required_extensions;
}

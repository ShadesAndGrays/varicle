#include "device.hpp"
#include "core/context.hpp"
#include "vulkan/vulkan.hpp"
#include <optional>
#include <print>
#include <set>

namespace varicle::render::vulkan {

void select_physical_device(VulkanContext& ctx) {

    auto physical_devices = ctx.m_instance.enumeratePhysicalDevices();
    if (physical_devices.empty()) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    auto const dev_iter =
        std::ranges::find_if(physical_devices, [&](auto const& physicalDevice) {
            return is_device_suitable(physicalDevice);
        });
    if (dev_iter == physical_devices.end()) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    ctx.m_physical_device = *dev_iter;
}
void create_logical_device(VulkanContext& ctx) {

    ctx.m_indices =
        find_queue_families_indices(ctx.m_physical_device, ctx.m_surface);
    std::set<uint32_t> unique_queue_familes = {
        ctx.m_indices.graphics_family.value(),
        ctx.m_indices.present_family.value(),
        ctx.m_indices.transfer_family.value()
    };

    vk::StructureChain<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        feature_chain = { {.features = {.samplerAnisotropy = true}},
                          { .shaderDrawParameters = true },
                          { .synchronization2 = true,
                            .dynamicRendering = true },
                          { .extendedDynamicState = true } };

    // These are device (per logical device) specific extensions
    std::vector<const char*> required_device_extension = {
        vk::KHRSwapchainExtensionName
    };

    std::vector<vk::DeviceQueueCreateInfo> queue_create_info;

    float queue_priority = 1.0f;

    for (uint32_t queue_family : unique_queue_familes) {
        queue_create_info.push_back(
            {
                .queueFamilyIndex = queue_family,
                .queueCount       = 1,
                .pQueuePriorities = &queue_priority,
            }
        );
    }

    vk::DeviceCreateInfo device_create_info{
        .pNext = &feature_chain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size()),
        .pQueueCreateInfos    = queue_create_info.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(required_device_extension.size()),
        .ppEnabledExtensionNames = required_device_extension.data()
    };

    // Take note of no instance parameter. Only interacts with
    // physical device. Queues are created automatically
    ctx.m_device = ctx.m_physical_device.createDevice(device_create_info);

    // This get's our queue handle from the device
    ctx.m_graphics_queue =
        ctx.m_device.getQueue(ctx.m_indices.graphics_family.value(), 0);
    ctx.m_present_queue =
        ctx.m_device.getQueue(ctx.m_indices.present_family.value(), 0);
    ctx.m_transfer_queue =
        ctx.m_device.getQueue(ctx.m_indices.transfer_family.value(), 0);
    std::println(
        "grap_que: {} , preset_que: {} ,trans_que: {} ",
        ctx.m_indices.graphics_family.value(),
        ctx.m_indices.present_family.value(),
        ctx.m_indices.transfer_family.value()
    );
}

bool is_device_suitable(vk::PhysicalDevice const& physical_device) {
    bool supports_vulkan1_3 =
        physical_device.getProperties().apiVersion >= vk::ApiVersion13;
    auto queue_families = physical_device.getQueueFamilyProperties();
    bool support_graphics =
        std::ranges::any_of(queue_families, [](auto const& qfp) {
            return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

    std::vector<const char*> required_device_extensions = {
        vk::KHRSwapchainExtensionName, vk::KHRSynchronization2ExtensionName
    };

    auto available_device_extensions =
        physical_device.enumerateDeviceExtensionProperties();

    bool supports_all_required_extensions = std::ranges::all_of(
        required_device_extensions,
        [&available_device_extensions](auto const& required_device_extension) {
            return std::ranges::any_of(
                available_device_extensions,
                [required_device_extension](
                    auto const& available_device_extension
                ) {
                    return strcmp(
                               available_device_extension.extensionName,
                               required_device_extension
                           ) == 0;
                }
            );
        }
    );

    // Returns a Structure chain. physicalDevice.template tells
    // compiler this is a templated function or something
    auto feature = physical_device.template getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    bool supports_required_features =
        feature.template get<vk::PhysicalDeviceFeatures2>()
            .features.samplerAnisotropy &&
        feature.template get<vk::PhysicalDeviceVulkan11Features>()
            .shaderDrawParameters &&
        feature.template get<vk::PhysicalDeviceVulkan13Features>()
            .dynamicRendering &&
        feature.template get<vk::PhysicalDeviceVulkan13Features>()
            .synchronization2 &&
        feature
            .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
            .extendedDynamicState;

    return support_graphics && supports_all_required_extensions &&
        supports_required_features && supports_vulkan1_3;
}

void list_devices(VulkanContext& ctx) {
    auto physical_devices = ctx.m_instance.enumeratePhysicalDevices();
    if (physical_devices.empty()) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    // List devices
    std::println("Devices Found: ");
    for (auto physical_device : physical_devices) {
        std::println(
            "\t {} ", std::string(physical_device.getProperties().deviceName)
        );
        for (auto supportedExtension :
             physical_device.enumerateDeviceExtensionProperties()) {
            std::println(
                "\t\t {} ", std::string(supportedExtension.extensionName)
            );
        }
    }
}

QueueFamilyIndices find_queue_families_indices(
    vk::PhysicalDevice physical_device,
    vk::SurfaceKHR     surface
) {

    std::vector<vk::QueueFamilyProperties> queue_families_properties =
        physical_device.getQueueFamilyProperties();

    QueueFamilyIndices indices;

    for (uint32_t qfp_index = 0; qfp_index < queue_families_properties.size();
         qfp_index += 1) {

        vk::QueueFlags flags = queue_families_properties[qfp_index].queueFlags;

        VkBool32 can_present =
            physical_device.getSurfaceSupportKHR(qfp_index, surface);
        if (can_present) {
            indices.present_family = qfp_index;
        }

        if (flags & vk::QueueFlagBits::eGraphics) {
            indices.graphics_family = qfp_index;
        }

        // prefer transfers without graphics
        if (flags & vk::QueueFlagBits::eTransfer &&
            !(flags & vk::QueueFlagBits::eGraphics)) {
            indices.transfer_family = qfp_index;
        }

        if (indices.is_complete()) {
            break;
        }
    }

    if (!indices.transfer_family.has_value()) {
        for (uint32_t j = 0; j < queue_families_properties.size(); ++j) {
            if (queue_families_properties[j].queueFlags &
                vk::QueueFlagBits::eTransfer) {
                indices.transfer_family = j;
                break;
            }
        }
    }

    return indices;
}
} // namespace varicle::render::vulkan

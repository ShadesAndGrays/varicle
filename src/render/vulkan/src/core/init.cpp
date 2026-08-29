#include "core/context.hpp"
#include "vulkan-lean.hpp"
#if !defined(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <print>

// for resolving function pointers at runtime
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

namespace varicle::render::vulkan {

struct VulkanRenderer::Impl {
    VulkanContext v_context;
};

GLFWwindow* init_window(
    VulkanContext* context,
    uint32_t       width  = 800,
    uint32_t       height = 600,
    const char*    name   = "Varicle"
);

vk::ApplicationInfo create_app();

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*> &validation_layers,
    vk::ApplicationInfo*           p_app_info,
    std::vector<const char*>       &required_layers,
    std::vector<const char*>       &required_extensions
);

vk::Instance create_instance();

std::vector<char const*> get_required_extensions();

std::vector<char const*>
get_required_layers(std::vector<const char*> &validation_layers);

namespace debug {
void setupDebugMessenger(VulkanContext& context);
}

} // namespace varicle::render::vulkan

// IMPLEMENTATION

namespace varicle::render::vulkan {

std::vector<char const*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};

void VulkanRenderer ::init(
    void*    native_window_handle,
    uint32_t width,
    uint32_t height
) {
    impl = new Impl{};

    std::println("VulkanRender: Initializing Vulkan");

    vk::detail::defaultDispatchLoaderDynamic.init(impl->v_context.m_dl);
    impl->v_context.m_window = init_window(&impl->v_context);

    impl->v_context.m_instance = create_instance();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(impl->v_context.m_instance);

    if (enable_validation_layers)
        debug::setupDebugMessenger(impl->v_context);
}

GLFWwindow* init_window(
    VulkanContext* context,
    uint32_t       width,
    uint32_t       height,
    const char*    name
) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    auto window = glfwCreateWindow(width, height, name, nullptr, nullptr);

    glfwSetWindowUserPointer(window, context);
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow* window, int width, int height) {
            auto ctx =
                static_cast<VulkanContext*>(glfwGetWindowUserPointer(window));
            if (ctx) {
                ctx->m_framebufferResized = true;
            }
        }
    );

    return window;
}

vk::Instance create_instance() {

    try {

        // create the application info
        vk::ApplicationInfo app_info = create_app();

        // get required layers and extensions from the
        auto required_layers     = get_required_layers(validation_layers);
        auto required_extensions = get_required_extensions();

        vk::InstanceCreateInfo instance_create_info = create_instance_info(
            validation_layers, &app_info, required_layers, required_extensions

        );

        vk::Instance instance =
            vk::createInstance(instance_create_info, nullptr);
        vk::detail::defaultDispatchLoaderDynamic.init(instance);
        return instance;
    } catch (vk::SystemError const& err) {
        std::println("vk::SystemError: {}", err.what());
        std::exit(-1);
    } catch (...) {
        std::println("Unknown error");
        std::exit(-1);
    }
    // return instance;
}

vk::ApplicationInfo create_app() {
    constexpr vk::ApplicationInfo appInfo{ .pApplicationName = "Hello Triangle",
                                           .applicationVersion =
                                               VK_MAKE_VERSION(1, 0, 0),
                                           .pEngineName = "No Engine",
                                           .engineVersion =
                                               VK_MAKE_VERSION(1, 0, 0),
                                           .apiVersion = vk::ApiVersion14 };
    return appInfo;
}

std::vector<const char*> getRequiredInstanceExtenstions() {
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

std::vector<const char*>
get_required_layers(std::vector<char const*> &validation_layers) {

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

std::vector<char const*> get_required_extensions() {

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

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*> &validation_layers,
    vk::ApplicationInfo*           p_app_info,
    std::vector<const char*>       &required_layers,
    std::vector<const char*>       &required_extensions
) {

    vk::InstanceCreateInfo create_info{
        .pApplicationInfo    = p_app_info,
        .enabledLayerCount   = static_cast<uint32_t>(required_layers.size()),
        .ppEnabledLayerNames = required_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size()),
        .ppEnabledExtensionNames = required_extensions.data()
    };

    return create_info;
}

bool VulkanRenderer::should_close_window() {
    return glfwWindowShouldClose(impl->v_context.m_window);
}

void VulkanRenderer::shutdown() {
    std::println("Shutting down....");

    glfwDestroyWindow(impl->v_context.m_window);
    glfwTerminate();

    // call last
    if (impl != nullptr)
        delete impl;
}

} // namespace varicle::render::vulkan

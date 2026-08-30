#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/init.hpp"
#include "core/config.hpp"
#include "core/context.hpp"
#include "core/validation.hpp"

#include <print>

// for resolving function pointers at runtime
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// IMPLEMENTATION

namespace varicle::render::vulkan {

static void
    framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
        auto ctx = reinterpret_cast<VulkanContext*>(
            glfwGetWindowUserPointer(window)
        );
        ctx->m_framebuffer_resized = true;
    }


void init_window(
    VulkanContext& ctx,
    uint32_t       width,
    uint32_t       height,
    const char*    name
) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    ctx.m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);

    glfwSetWindowUserPointer(ctx.m_window, &ctx);
    glfwSetFramebufferSizeCallback(ctx.m_window,framebuffer_resize_callback);

}


void create_instance(VulkanContext& ctx) {

    try {

        vk::detail::defaultDispatchLoaderDynamic.init(ctx.m_dl);
        // create the application info
        vk::ApplicationInfo app_info = create_app();

        // get required layers and extensions from the
        auto required_layers     = get_required_layers();
        auto required_extensions = get_required_extensions();

        vk::InstanceCreateInfo instance_create_info = create_instance_info(
            validation_layers, &app_info, required_layers, required_extensions

        );

        vk::Instance instance =
            vk::createInstance(instance_create_info, nullptr);
        vk::detail::defaultDispatchLoaderDynamic.init(instance);
        ctx.m_instance = instance;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(ctx.m_instance);
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

vk::InstanceCreateInfo create_instance_info(
    const std::vector<char const*>& validation_layers,
    vk::ApplicationInfo*            p_app_info,
    std::vector<const char*>&       required_layers,
    std::vector<const char*>&       required_extensions
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

void create_surface(VulkanContext& ctx) {

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkResult result = glfwCreateWindowSurface(
        ctx.m_instance, ctx.m_window, nullptr, &surface
    );
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    ctx.m_surface = vk::SurfaceKHR(surface);
}

} // namespace varicle::render::vulkan

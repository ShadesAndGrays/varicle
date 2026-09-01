#include "core/swap-chain.hpp"
#include "graphics/image.hpp"
#include <print>

namespace varicle::render::vulkan {

void create_swap_chain(VulkanContext& ctx) {

    auto surface_capabilities =
        ctx.m_physical_device.getSurfaceCapabilitiesKHR(ctx.m_surface);

    ctx.m_swap_chain_extent = choose_swap_extent(ctx, surface_capabilities);
    uint32_t min_image_count =
        choose_swap_min_image_count(surface_capabilities);

    std::vector<vk::SurfaceFormatKHR> available_formats =
        ctx.m_physical_device.getSurfaceFormatsKHR(ctx.m_surface);

    ctx.m_swap_chain_surface_format =
        choose_swap_surface_format(available_formats);

    std::vector<vk::PresentModeKHR> available_present_modes =
        ctx.m_physical_device.getSurfacePresentModesKHR(ctx.m_surface);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface          = ctx.m_surface,
        .minImageCount    = min_image_count,
        .imageFormat      = ctx.m_swap_chain_surface_format.format,
        .imageColorSpace  = ctx.m_swap_chain_surface_format.colorSpace,
        .imageExtent      = ctx.m_swap_chain_extent,
        .imageArrayLayers = 1,
        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform     = surface_capabilities.currentTransform,
        .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode      = choose_swap_present_mode(available_present_modes),
        .clipped          = true
    };

    ctx.m_swap_chain = ctx.m_device.createSwapchainKHR(swapChainCreateInfo);
    ctx.m_swap_chain_images =
        ctx.m_device.getSwapchainImagesKHR(ctx.m_swap_chain);
    std::println(
        "number of swapchain images: {}", ctx.m_swap_chain_images.size()
    );
}

uint32_t choose_swap_min_image_count(
    vk::SurfaceCapabilitiesKHR const& surfaceCapabilities
) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

vk::SurfaceFormatKHR choose_swap_surface_format(
    std::vector<vk::SurfaceFormatKHR> const& availableFormats
) {
    // TODO: Better error handling here
    assert(!availableFormats.empty());

    const auto formatIt =
        std::ranges::find_if(availableFormats, [](const auto& format) {
            return format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR choose_swap_present_mode(
    std::vector<vk::PresentModeKHR> const& availablePresentModes
) {

    // Should be guaranteed
    assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
        return presentMode == vk::PresentModeKHR::eFifo;
    }));

    return std::ranges::any_of(
               availablePresentModes,
               [](const vk::PresentModeKHR value) {
                   return vk::PresentModeKHR::eMailbox == value;
               }
           )
        ? vk::PresentModeKHR::eMailbox
        : vk::PresentModeKHR::eFifo;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D choose_swap_extent(
    VulkanContext&                    ctx,
    vk::SurfaceCapabilitiesKHR const& capabilities
) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(ctx.m_window, &width, &height);
    return {
        std::clamp<uint32_t>(
            width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        ),
        std::clamp<uint32_t>(
            height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        ),
    };
}

void create_image_views(VulkanContext& ctx) {
    assert(ctx.m_swap_chain_image_views.empty());

    for (auto& image : ctx.m_swap_chain_images) {
        ctx.m_swap_chain_image_views.emplace_back(
                create_image_view(ctx,image,ctx.m_swap_chain_surface_format.format)
        );
    }
}

void cleanup_swap_chain(VulkanContext& ctx) {
    for (auto& view : ctx.m_swap_chain_image_views) {
        ctx.m_device.destroyImageView(view);
    }
    ctx.m_swap_chain_image_views.clear();
    ctx.m_device.destroySwapchainKHR(ctx.m_swap_chain);
    ctx.m_swap_chain = nullptr;
}

void recreate_swap_chain(VulkanContext& ctx) {

    int width = 0, height = 0;
    do {
        glfwGetFramebufferSize(ctx.m_window, &width, &height);
        glfwWaitEvents();
    } while (width == 0 || height == 0);

    ctx.m_device.waitIdle();

    cleanup_swap_chain(ctx);

    create_swap_chain(ctx);
    create_image_views(ctx);
}

} // namespace varicle::render::vulkan

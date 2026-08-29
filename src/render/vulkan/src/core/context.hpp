#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "vulkan-lean.hpp"

namespace varicle::render::vulkan {

struct VulkanContext {

    GLFWwindow* m_window             = nullptr;
    uint32_t    m_queueIndex         = 0; // ~0 = flip 0000000 = 0xFFFFFF
    uint32_t    m_frameIndex         = 0;
    bool        m_framebufferResized = false;

    // Vulkan member variables
    vk::Instance               m_instance       = nullptr;
    vk::DebugUtilsMessengerEXT m_debugMessanger = nullptr;
    vk::SurfaceKHR             m_surface        = nullptr;
    vk::PhysicalDevice         m_physicalDevice = nullptr;
    vk::Device                 m_device         = nullptr;

    vk::SwapchainKHR           m_swapChain = nullptr;
    std::vector<vk::Image>     m_swapChainImages;
    vk::SurfaceFormatKHR       m_swapChainSurfaceFormat;
    vk::Extent2D               m_swapChainExtent;
    std::vector<vk::ImageView> m_swapChainImageViews;

    vk::PipelineLayout m_pipelineLayout   = nullptr;
    vk::Pipeline       m_graphicsPipeline = nullptr;

    vk::CommandPool                m_commandPool = nullptr;
    std::vector<vk::CommandBuffer> m_commandBuffers;

    std::vector<vk::Semaphore> m_presentCompleteSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence>     m_inFlightFences;

    vk::Buffer       m_vertexBuffer       = nullptr; // interface for memory
    vk::DeviceMemory m_vertexBufferMemory = nullptr; // actually memory

    // These are automatically cleaned up when the device is destroyed
    vk::Queue m_queue = nullptr; // for graphics and presenting
                                 //
                    
    vk::detail::DynamicLoader m_dl;
};

} // namespace varicle::render

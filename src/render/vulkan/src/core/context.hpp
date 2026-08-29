#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

struct VulkanContext {

    GLFWwindow* m_window             = nullptr;
    uint32_t    m_queueIndex         = 0; // ~0 = flip 0000000 = 0xFFFFFF
    uint32_t    m_frameIndex         = 0;
    bool        m_framebufferResized = false;

    // Vulkan member variables
    // These are automatically cleaned up when the object is destroyed
    // VkContext                m_context;
    vk::raii::Instance               m_instance       = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessanger = nullptr;
    vk::raii::SurfaceKHR             m_surface        = nullptr;
    vk::raii::PhysicalDevice         m_physicalDevice = nullptr;
    vk::raii::Device                 m_device         = nullptr;

    vk::raii::SwapchainKHR           m_swapChain = nullptr;
    std::vector<vk::Image>           m_swapChainImages;
    vk::SurfaceFormatKHR             m_swapChainSurfaceFormat;
    vk::Extent2D                     m_swapChainExtent;
    std::vector<vk::raii::ImageView> m_swapChainImageViews;

    vk::raii::PipelineLayout m_pipelineLayout   = nullptr;
    vk::raii::Pipeline       m_graphicsPipeline = nullptr;

    vk::raii::CommandPool                m_commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;

    std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence>     m_inFlightFences;

    vk::raii::Buffer       m_vertexBuffer = nullptr; // interface for memory
    vk::raii::DeviceMemory m_vertexBufferMemory = nullptr; // actually memory

    // These are automatically cleaned up when the device is destroyed
    vk::raii::Queue m_queue = nullptr; // for graphics and presenting
};


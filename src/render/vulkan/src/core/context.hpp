#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace varicle::render::vulkan {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;
    std::optional<uint32_t> transfer_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value() &&
            transfer_family.has_value();
    }
};

struct VulkanContext {

    GLFWwindow*         m_window = nullptr;
    QueueFamilyIndices  m_indices;
    uint32_t            m_frame_index         = 0;
    bool                m_framebuffer_resized = false;
    uint32_t            m_image_index         = 0;
    bool                m_recreating_frame    = false;
    vk::ClearColorValue m_clear_color =
        vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    // Vulkan member variables
    vk::Instance               m_instance        = nullptr;
    vk::DebugUtilsMessengerEXT m_debugMessenger  = nullptr;
    vk::SurfaceKHR             m_surface         = nullptr;
    vk::PhysicalDevice         m_physical_device = nullptr;
    vk::Device                 m_device          = nullptr;

    vk::SwapchainKHR           m_swap_chain = nullptr;
    std::vector<vk::Image>     m_swap_chain_images;
    vk::SurfaceFormatKHR       m_swap_chain_surface_format;
    vk::Extent2D               m_swap_chain_extent;
    std::vector<vk::ImageView> m_swap_chain_image_views;

    vk::DescriptorSetLayout        m_descriptor_set_layout = nullptr;
    vk::PipelineLayout             m_pipeline_layout       = nullptr;
    vk::Pipeline                   m_graphics_pipeline     = nullptr;
    vk::ShaderModule               m_shader_module         = nullptr;
    vk::DescriptorPool             m_descriptor_pool       = nullptr;
    std::vector<vk::DescriptorSet> m_descriptor_sets;

    vk::CommandPool                m_command_pool = nullptr;
    std::vector<vk::CommandBuffer> m_command_buffers;

    vk::CommandPool                m_transfer_command_pool = nullptr;
    std::vector<vk::CommandBuffer> m_transfer_command_buffers;

    std::vector<vk::Semaphore> m_present_complete_semaphores;
    std::vector<vk::Semaphore> m_render_finished_semaphores;
    std::vector<vk::Fence>     m_in_flight_fences;

    vk::Buffer       m_vertex_buffer        = nullptr; // interface for memory
    vk::DeviceMemory m_vertex_buffer_memory = nullptr; // actually memory
                                                       //
    vk::Buffer       m_index_buffer        = nullptr;  // interface for memory
    vk::DeviceMemory m_index_buffer_memory = nullptr;  // actually memory

    std::vector<vk::Buffer>       m_uniform_buffers;
    std::vector<vk::DeviceMemory> m_uniform_buffers_memory;
    std::vector<void*>            m_uniform_buffers_mapped;

    // I assume this should be would be from a
    // pool of memory in our custom allocator
    vk::Image        m_texture_image        = nullptr;
    vk::ImageView    m_texture_image_view   = nullptr;
    vk::Sampler      m_texture_sampler      = nullptr;
    vk::DeviceMemory m_texture_image_memory = nullptr;

    // These are automatically cleaned up when the device is destroyed
    vk::Queue m_graphics_queue = nullptr; // for graphics and presenting
    vk::Queue m_present_queue =
        nullptr; // for presenting / most cases present == graphics
    vk::Queue m_transfer_queue = nullptr; // for transfer only

    vk::detail::DynamicLoader m_dl;
};

} // namespace varicle::render::vulkan


#pragma once
#include "renderer/common/camera.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace varicle::renderer::vulkan {

struct alignas(16) FrameData {
    glm::mat4 viewProj  = glm::mat4(1);
    glm::vec4 cameraPos = glm::vec4(1);
};

struct RenderCamera {
    varicle::renderer::Camera camera;

    UniformBufferData<FrameData>                        frame_data;
    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptor_sets;

    RenderCamera(VulkanContext& ctx) {

        frame_data = UniformBufferData<FrameData>::create_uniform_buffer(ctx);
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            update(i);
        }
    }

    void update(uint32_t current_frame) {
        frame_data.update_uniform_buffer(
            { .viewProj =
                  camera.get_view_project_matrix(GraphicsBackend::Vulkan),
              .cameraPos = glm::vec4(
                  camera.position.x, camera.position.y, camera.position.z, 1.0f
              ) },
            current_frame
        );
    }

    std::vector<vk::DescriptorBufferInfo> get_buffer_descriptor() {
        std::vector<vk::DescriptorBufferInfo> result;
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            result.push_back(frame_data.get_buffer_info(i));
        }
        return result;
    }

    // Destroy gpu resources inside camera
    void destory(vk::Device& device, DescriptorSystem ds, uint32_t pool_idx) {
        UniformBufferData<FrameData>::destroy_uniform_buffer(
            device, std::move(frame_data)
        );
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            ds.pool_allocator.deallocate(device, descriptor_sets[i], pool_idx);
        }
    }
};

} // namespace varicle::renderer::vulkan

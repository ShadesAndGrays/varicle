
#pragma once
#include "core/type.hpp"
#include "renderer/common/camera.hpp"
#include "renderer/vulkan/core/config.hpp"
#include "renderer/vulkan/graphics/pipeline.hpp"
#include "renderer/vulkan/graphics/resource.hpp"
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace varicle::renderer::vulkan {

// Even though we want to have two frame data in total, descriptor count
// remains 1. This is what we want are shader to actuall map against.
// setting this to two would map and array of size 2 for our frame data
const auto frame_data_layout_set_binding =
    vk::DescriptorSetLayoutBinding{ 0,
                                    vk::DescriptorType::eUniformBuffer,
                                    1,
                                    vk::ShaderStageFlagBits::eVertex |
                                        vk::ShaderStageFlagBits::eFragment };

struct alignas(16) FrameData {
    glm::mat4 viewProj  = glm::mat4(1);
    glm::vec4 cameraPos = glm::vec4(1);
};

struct RenderCamera {
    varicle::renderer::Camera camera;

    UniformBufferData<FrameData>        frame_data;
    vk::DescriptorSetLayout             frame_data_layout_set;
    DescriptorPoolAllocator::Allocation descriptor_pool_alloc;

    RenderCamera(
        VulkanContext&   ctx,
        DescriptorSystem& ds,
        Vec3             initial_position = { 0, 0, 2 },
        Camera c = varicle::renderer::Camera::default_orthographic()
    )
        : camera(std::move(c)) {

        camera.position = initial_position;

        frame_data = UniformBufferData<FrameData>::create_uniform_buffer(ctx);

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            update(i);
        }

        frame_data_layout_set = ds.cache.get_or_create_layout(
            ctx.m_device,
            {
                frame_data_layout_set_binding,
            }
        );

        descriptor_pool_alloc = ds.pool_allocator.allocate(
            ctx.m_device, { frame_data_layout_set, frame_data_layout_set }
        );

        ds.writer.write_buffer(
            ctx.m_device,
            frame_data_layout_set_binding,
            descriptor_pool_alloc.descriptor_sets[0],
            std::array{ get_buffer_descriptor()[0] }
        );

        ds.writer.write_buffer(
            ctx.m_device,
            frame_data_layout_set_binding,
            descriptor_pool_alloc.descriptor_sets[1],
            std::array{ get_buffer_descriptor()[1] }
        );
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
    void destory(vk::Device& device, DescriptorSystem& ds) {
        UniformBufferData<FrameData>::destroy_uniform_buffer(
            device, std::move(frame_data)
        );
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            ds.pool_allocator.deallocate(
                device,
                descriptor_pool_alloc.descriptor_sets[i],
                descriptor_pool_alloc.descriptor_pool_idx
            );
        }
    }
};

} // namespace varicle::renderer::vulkan

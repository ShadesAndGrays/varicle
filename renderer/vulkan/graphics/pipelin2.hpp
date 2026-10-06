#pragma once

#include "renderer/vulkan/core/config.hpp"
#include "vulkan/vulkan.hpp"
#include <print>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

using std::println;

namespace varicle::renderer::vulkan {

struct LayoutKeyHash {
    size_t operator()(
        const std::vector<vk::DescriptorSetLayoutBinding>& bindings
    ) const {
        size_t hash = 0;

        for (const auto& b : bindings) {
            hash ^= std::hash<uint32_t>{}(b.binding) + 0x9e3779b9 +
                (hash << 6) + (hash >> 2);

            hash ^=
                std::hash<uint32_t>{}(static_cast<uint32_t>(b.descriptorType)) +
                0x9e3779b9 + (hash << 6) + (hash >> 2);

            hash ^= std::hash<uint32_t>{}(b.descriptorCount) + 0x9e3779b9 +
                (hash << 6) + (hash >> 2);

            // hash ^= std::hash<vk::ShaderStageFlags>{}(b.stageFlags) +
            // 0x9e3779b9 +
            //     (hash << 6) + (hash >> 2);
        }

        return hash;
    }
};

class DescriptorLayoutCache {
  private:
    std::unordered_map<
        std::vector<vk::DescriptorSetLayoutBinding>,
        vk::DescriptorSetLayout,
        LayoutKeyHash>
        layout_cache;

  public:
    vk::DescriptorSetLayout get_or_create_layout(
        vk::Device                                        device,
        const std::vector<vk::DescriptorSetLayoutBinding> bindings
    ) {
        if (layout_cache.contains(bindings)) {
            return layout_cache[bindings];
        }

        vk::DescriptorSetLayoutCreateInfo layout_info{
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings    = bindings.data()
        };

        layout_cache[bindings] = device.createDescriptorSetLayout(layout_info);

        return layout_cache[bindings];
    }
};

class DescriptorPoolAllocator {
  private:
    // some default setting
    static constexpr int s_maxSets  = 256;
    static constexpr int s_samplers = 1024;
    static constexpr int s_buffers  = 256;

    std::vector<vk::DescriptorPool> m_full;
    vk::DescriptorPool              m_current = nullptr;

  public:
    void create_new_pool(vk::Device device) {
        std::array<vk::DescriptorPoolSize, 2> pool_size{
            vk::DescriptorPoolSize{
                vk::DescriptorType::eCombinedImageSampler,
                s_samplers,
            },
            vk::DescriptorPoolSize{
                vk::DescriptorType::eUniformBuffer,
                s_samplers,
            }
        };

        vk::DescriptorPoolCreateInfo pool_info{
            .flags   = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = s_maxSets,
            .poolSizeCount = static_cast<uint32_t>(pool_size.size()),
            .pPoolSizes    = pool_size.data()
        };

        if (m_current != nullptr) {
            m_full.push_back(m_current);
        }
        m_current = device.createDescriptorPool(pool_info);
    }

    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> create_set(
        vk::Device                    device,
        vk::DescriptorPool            pool,
        vk::DescriptorSetAllocateInfo alloc_info
    ) {

        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> sets;
        auto v_sets = device.allocateDescriptorSets(alloc_info);
        std::copy(
            v_sets.begin(), v_sets.begin() + MAX_FRAMES_IN_FLIGHT, sets.begin()
        );

        return sets;
    }

    // checks sets is not empty
    std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT>
    allocate(vk::Device device, vk::DescriptorSetLayout layout) {

        std::vector<vk::DescriptorSetLayout> layouts(
            MAX_FRAMES_IN_FLIGHT, layout
        );

        vk::DescriptorSetAllocateInfo alloc_info{
            .descriptorPool     = m_current,
            .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
            .pSetLayouts        = layouts.data()
        };
        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> sets;

        try {
            sets = create_set(device, m_current, alloc_info);
        } catch (vk::OutOfPoolMemoryError e) {
            create_new_pool(device);
            sets = create_set(device, m_current, alloc_info);
        } catch (...) {
            println("out of memory. failed to create a new descriptor pool");
        }

        return sets;
    }
};

class DescriptorWriter {
  public:
    void write_buffer(
        vk::Device                          device,
        vk::DescriptorSetLayoutBinding      layout_binding,
        vk::DescriptorSet                   descriptor_set,
        std::span<vk::DescriptorBufferInfo> buffer_infos,
        std::uint32_t                       offset = 0

    ) {

        std::vector<vk::WriteDescriptorSet> writes;
        writes.push_back(
            { .dstSet          = descriptor_set,
              .dstBinding      = layout_binding.binding,
              .dstArrayElement = offset,
              .descriptorCount = static_cast<uint32_t>(buffer_infos.size()),
              .descriptorType  = layout_binding.descriptorType,
              .pBufferInfo     = buffer_infos.data() }
        );
    }

    void write_texture(
        vk::Device                         device,
        vk::DescriptorSetLayoutBinding     layout_binding,
        vk::DescriptorSet                  descriptor_set,
        std::span<vk::DescriptorImageInfo> image_infos,
        std::uint32_t                      offset = 0
    ) {
        std::vector<vk::WriteDescriptorSet> writes;

        writes.push_back(
            { .dstSet          = descriptor_set,
              .dstBinding      = layout_binding.binding,
              .dstArrayElement = offset,
              .descriptorCount = static_cast<uint32_t>(image_infos.size()),
              .descriptorType  = layout_binding.descriptorType,
              .pImageInfo      = image_infos.data() }
        );

        device.updateDescriptorSets(writes, {});
    }
};

struct DescriptorSystem {
    DescriptorLayoutCache   cache;
    DescriptorPoolAllocator pool_allocator;
    DescriptorWriter        writer;
};

static DescriptorSystem create_default_descriptor_system(vk::Device device) {
    DescriptorLayoutCache   lc{};
    DescriptorPoolAllocator dpa{};
    DescriptorWriter        writer{};
    return { std::move(lc), std::move(dpa), std::move(writer) };
}

} // namespace varicle::renderer::vulkan

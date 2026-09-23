#pragma once

#include <variant>

namespace varicle::renderer {

namespace opengl {
struct GLContext;
}

namespace vulkan {
struct VulkanContext;
}
namespace webgpu {
struct WebGPUContext;
}

template <typename... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

using RendererContext = std::variant<
    std::monostate,
    opengl::GLContext,
    vulkan::VulkanContext,
    webgpu::WebGPUContext>;
} // namespace varicle::renderer

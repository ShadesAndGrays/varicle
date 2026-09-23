#include "GLFW/glfw3.h"
#include <chrono>
#include <memory>
#include <print>
#include <thread>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu_cpp.h>
#endif

const int WIDTH  = 800;
const int HEIGHT = 600;

const char shaderCode[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

using std::println;

struct WebGPUContext {
    WebGPUContext()  = default;
    ~WebGPUContext() = default;

    WebGPUContext(const WebGPUContext&)            = delete;
    WebGPUContext& operator=(const WebGPUContext&) = delete;

    WebGPUContext(WebGPUContext&&)            = default;
    WebGPUContext& operator=(WebGPUContext&&) = default;

    wgpu::Instance      instance;
    wgpu::Adapter       adapter;
    wgpu::Device        device;
    wgpu::Surface       surface;
    wgpu::TextureFormat format;

    wgpu::RenderPipeline pipeline;
};

std::unique_ptr<WebGPUContext> global_ctx;

void init(WebGPUContext& ctx) {
    static const auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instance_desc = { .requiredFeatureCount = 1,
                                               .requiredFeatures =
                                                   &kTimedWaitAny };

    ctx.instance = wgpu::CreateInstance(&instance_desc);

    wgpu::Future f1 = ctx.instance.RequestAdapter(
        nullptr,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestAdapterStatus status,
            wgpu::Adapter              a,
            wgpu::StringView           message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                println("Request Adapter: {}", message.data);
                exit(0);
            }
            ctx.adapter = std::move(a);
        }
    );
    ctx.instance.WaitAny(f1, UINT64_MAX);

    wgpu::DeviceDescriptor desc{};

    desc.SetUncapturedErrorCallback([](const wgpu::Device&,
                                       wgpu::ErrorType  error_type,
                                       wgpu::StringView message) {
        println("Error: {} - message:{}", int(error_type), message.data);
    });

    wgpu::Future f2 = ctx.adapter.RequestDevice(
        &desc,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestDeviceStatus status,
            wgpu::Device              d,
            wgpu::StringView          message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                println("RequestDevice: {}", message.data);
                exit(0);
            }
            ctx.device = std::move(d);
        }
    );
    ctx.instance.WaitAny(f2, UINT64_MAX);
}

void configure_surface(WebGPUContext& ctx) {

    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvasDesc;
    canvasDesc.selector = "#canvas";

    wgpu::SurfaceDescriptor surface_desc{};
    surface_desc.nextInChain = &canvasDesc;
    ctx.surface              = ctx.instance.CreateSurface(&surface_desc);


    wgpu::SurfaceCapabilities capabilities;
    ctx.surface.GetCapabilities(ctx.adapter, &capabilities);
    ctx.format = capabilities.formats[0];

    wgpu::SurfaceConfiguration config{ .device = ctx.device,
                                       .format = ctx.format,
                                       .width  = WIDTH,
                                       .height = HEIGHT };
    ctx.surface.Configure(&config);
}

void create_render_pipeline(WebGPUContext& ctx) {
    wgpu::ShaderSourceWGSL       wgsl{ { .code = shaderCode } };
    wgpu::ShaderModuleDescriptor shader_module_descriptor{ .nextInChain =
                                                               &wgsl };

    wgpu::ShaderModule shader_module =
        ctx.device.CreateShaderModule(&shader_module_descriptor);

    wgpu::ColorTargetState color_target_state{ .format = ctx.format };

    wgpu::FragmentState fragment_state{ .module      = shader_module,
                                        .targetCount = 1,
                                        .targets     = &color_target_state };
    wgpu::RenderPipelineDescriptor descriptor{
        .vertex = { .module = shader_module }, .fragment = &fragment_state
    };

    ctx.pipeline = ctx.device.CreateRenderPipeline(&descriptor);
}

void init_graphics(WebGPUContext& ctx) {
    configure_surface(ctx);
    create_render_pipeline(ctx);
}

void render(WebGPUContext& ctx) {
    wgpu::SurfaceTexture surface_texture;
    ctx.surface.GetCurrentTexture(&surface_texture);

    if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
        println("GetCurrentTexture failed with status: {}", int(surface_texture.status));
        return; // skip this frame
    }

    auto clear_color = wgpu::Color{ 1.0, 1.0, 1.0, 1.0 };

    wgpu::RenderPassColorAttachment attachment{
        .view       = surface_texture.texture.CreateView(),
        .loadOp     = wgpu::LoadOp::Clear,
        .storeOp    = wgpu::StoreOp::Store,
        .clearValue = clear_color
    };

    wgpu::RenderPassDescriptor render_pass{ .colorAttachmentCount = 1,
                                            .colorAttachments = &attachment };

    wgpu::CommandEncoder    encoder = ctx.device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass    = encoder.BeginRenderPass(&render_pass);
    pass.SetPipeline(ctx.pipeline);
    pass.Draw(3);
    pass.End();

    wgpu::CommandBuffer commands = encoder.Finish();
    ctx.device.GetQueue().Submit(1, &commands);
}

void varicle_sleep(long ms) {
#ifdef __EMSCRIPTEN__
    emscripten_sleep(ms);
#else
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1ms * ms);
#endif
}

void loop(void) {
    auto ctx = global_ctx.get();
    glfwPollEvents();
    render(*ctx);
    // ctx->surface.Present();
    ctx->instance.ProcessEvents();
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);

#ifdef __EMSCRIPTEN__
    global_ctx = std::make_unique<WebGPUContext>();
    // ctx.surface = wgpu::glfw::CreateSurfaceForWindow(ctx.instance, window);

    init(*global_ctx);
    init_graphics(*global_ctx);
    emscripten_set_main_loop(loop, 64, false);
#else
    while (!glfwWindowShouldClose(window)) {
        loop();
    }
#endif

    glfwDestroyWindow(window);
}

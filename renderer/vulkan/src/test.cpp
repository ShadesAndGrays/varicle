#include "graphics/pipeline.hpp"
#include "vulkan-renderer.hpp"
#include <GLFW/glfw3.h>
#include <print>
#include <vector>

const char* MODEL_PATH   = "models/cube.obj";
const char* TEXTURE_PATH = "textures/cube.png";

bool should_clear_screen = true;

void key_callback(
    GLFWwindow* window,
    int         key,
    int         scancode,
    int         action,
    int         mods
) {
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        should_clear_screen = !should_clear_screen;
    }
}

void run() {
    using namespace varicle::render;

    std::unique_ptr<vulkan::VulkanRenderer> render =
        std::make_unique<vulkan::VulkanRenderer>();

    // auto dummy_device = vk::Device{ nullptr };
    //
    // // Pipeline manager should be created inside renderer
    // vulkan::PipelineCreationSystem pm;
    //
    // struct VignetteBufferObject {
    //     glm::vec4    tint{ 0.0f, 0.0f, 0.0f, 1.0f };
    //     glm::vec2    offset_ratio{ 0.5f, 0.5f };
    //     glm::float32 strength{ 0.5f };
    // };
    //
    // // Bindings needs to be created by the user
    // // What the shader will have
    // std::vector<vk::DescriptorSetLayoutBinding>
    //     default_descriptor_layout_binding = {
    //         {
    //             .binding         = 0,
    //             .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
    //             .descriptorCount = 64,
    //             .stageFlags      = vk::ShaderStageFlagBits::eFragment,
    //         },
    //     };
    //
    // std::vector<vk::DescriptorSetLayoutBinding> vignette_layout_binding = {
    //     {
    //         .binding         = 1,
    //         .descriptorType  = vk::DescriptorType::eUniformBuffer,
    //         .descriptorCount = 1,
    //         .stageFlags      = vk::ShaderStageFlagBits::eFragment,
    //     },
    // };
    //
    // // GPU Object
    // vk::DescriptorSetLayout layout = pm.create_descriptor_set_layout(
    //     dummy_device, default_descriptor_layout_binding
    // );
    //
    // vk::DescriptorSetLayout layout2 = pm.create_descriptor_set_layout(
    //     dummy_device, vignette_layout_binding
    // );
    //
    // vk::DescriptorPool pool = pm.calculate_descriptor_pool(dummy_device);

    // The GPU reads this each gpu pipeline flow
    // It's a form the GPU can read from
    // Multiple shaders  can share it if the strucure they need is then same and
    // the data is the same, else create a new set

    vk::DescriptorSet set; // we can put values into this

    Camera& camera    = render->get_camera();
    camera.far        = 100.0f;
    camera.position.z = 2.0f;
    camera.target.x   = 0.0f;
    camera.target.y   = 0.0f;

    // window.init(800, 600, "Varicle");
    render->init();
    // glfwSetKeyCallback(render->get_window(), key_callback);

    auto mesh = render->load_mesh(MODEL_PATH);

    render->set_clear_color(Color{ 1, 1, 0.7, 1 });

    // vulkan::PipelineBuilder pipeline_builder;

    Object obj1{ { 2.0f, 2.0f, 0.0f },
                 { 0.0f, 0.0f, 0.0f },
                 {
                     1.0f,
                     1.0f,
                     1.0f,
                 },
                 mesh };

    Object obj2   = obj1;
    obj2.position = { 5.0f, 3.0f, 1.0f };
    Object obj3   = obj1;
    obj3.position = { -3.0f, -3.0f, 2.0f };
    obj3.rotation = { 3.0f, 1.0f, 2.0f };
    Object obj4   = obj1;
    obj4.position = { -3.0f, 3.0f, 2.0f };
    obj4.rotation = { -3.0f, -3.0f, 2.0f };

    struct AnimatedBox {
        Object& obj;
        float   rot_x_multipler = 0.0f;
        float   rot_y_multipler = 0.0f;
        float   rot_z_multipler = 0.0f;
        float   time_offset     = 0.0f;
    };
    AnimatedBox boxes[] = { { obj1, 0, 1, 0, 15.0f },
                            { obj2, -2, 0, 1, 2.0f },
                            { obj3, 1, 0, -1, 3.0f },
                            { obj4, -1, -1, -1, 100.0f } };

    while (!render->should_close_window()) {
        camera.aspect = render->get_aspect();

        for (auto& i : boxes) {
            i.obj.rotation.x += 0.016f * i.rot_x_multipler;
            i.obj.rotation.y += 0.016f * i.rot_y_multipler;
            i.obj.rotation.z += 0.016f * i.rot_z_multipler;
            i.obj.position.z = sin(glfwGetTime() * 2 + i.time_offset) * 1;
        }

        render->begin_frame(should_clear_screen);

        // render->draw_object(object);
        for (auto& i : boxes) {
            render->draw_object(i.obj);
        }
        render->end_frame();
    }
    render->shutdown();
}

int main() {

    try {
        run();
    } catch (const std::exception& e) {
        std::println(stderr, "FATAL: {}", e.what());
    }
    return 0;
}

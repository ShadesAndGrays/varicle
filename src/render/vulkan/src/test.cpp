#include "vulkan-renderer.hpp"
#include <GLFW/glfw3.h>
#include <print>

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

    std::unique_ptr<IRender> render =
        std::make_unique<vulkan::VulkanRenderer>();
    Camera& camera = render->get_camera();
    camera.far     = 1000.0f;
    camera.position.z = 20.0f;
    camera.target.x = -30.0f;
    camera.target.y = -30.0f;

    render->init(800, 800, "Varicle");
    glfwSetKeyCallback(render->get_window(), key_callback);
    render->load_mesh(MODEL_PATH);

    render->set_clear_color(Color{ 1, 1, 0.7, 1 });
    while (!render->should_close_window()) {
        render->begin_frame(should_clear_screen);

        for (float i = 0.0f; i < 30; i += 1) {
            for (float j = 0.0f; j < 30; j += 1) {
                render->draw_mesh(
                    { 2.0f * -i, 2.0f * -j, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                    {
                        1.0f,
                        1.0f,
                        1.0f,
                    },
                    1

                );
            }
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

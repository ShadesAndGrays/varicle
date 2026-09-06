#include "vulkan-renderer.hpp"
#include <GLFW/glfw3.h>
#include <print>

const char * MODEL_PATH   = "models/cube.obj";
const char * TEXTURE_PATH = "textures/cube.png";


bool should_clear_screen = true;

 void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
     if (key == GLFW_KEY_E && action == GLFW_PRESS) {
         should_clear_screen = !should_clear_screen;
    }
}

void run() {
    using namespace varicle::render;

    std::unique_ptr<IRender> render = std::make_unique<vulkan::VulkanRenderer>() ;


    

    render->init(800, 600, "Varicle");
    glfwSetKeyCallback(render->get_window(),key_callback);
    // render->load_mesh(MODEL_PATH);

    render->set_clear_color(Color{ 1, 1, 0.7, 1 });
    while (!render->should_close_window()) {
        render->begin_frame(should_clear_screen);
        render->draw_mesh(0);
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

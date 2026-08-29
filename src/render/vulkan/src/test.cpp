#include "vulkan-lean.hpp"
#include <GLFW/glfw3.h>
#include <print>

void run() {
    using namespace varicle::render;

    IRender* render = new vulkan::VulkanRenderer();
    render->init(nullptr, 800, 600);

    while (!render->should_close_window()) {
        render->begin_frame();
        glfwPollEvents();

        render->end_frame();
    }
    render->shutdown();
    delete render;
}

int main() {

    try {
        run();
    } catch (const std::exception& e) {
        std::println(stderr, "FATAL: {}", e.what());
    }
    return 0;
}

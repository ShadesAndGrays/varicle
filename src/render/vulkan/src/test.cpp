#include "vulkan-renderer.hpp"
#include <GLFW/glfw3.h>
#include <print>

void run() {
    using namespace varicle::render;

    IRender* render = new vulkan::VulkanRenderer();
    render->init(800, 600,"Varicle");

    while (!render->should_close_window()) {
        render->begin_frame();
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

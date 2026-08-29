#include "vulkan-lean.hpp"

int main() {
    using namespace varicle::render;
    IRender* render = new VulkanRenderer();
    render->init(nullptr,800,600);
    delete render;
    return 0;
}

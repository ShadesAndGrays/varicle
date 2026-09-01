#pragma once
#include "core/context.hpp"
#include <string>

namespace varicle::render::vulkan {

// const std::string MODEL_PATH   = "models/viking_room.obj";
// const std::string TEXTURE_PATH = "textures/viking_room.png";

const std::string MODEL_PATH   = "models/cube.obj";
const std::string TEXTURE_PATH = "textures/cube.png";

void load_model(VulkanContext& ctx);

} // namespace varicle::render::vulkan

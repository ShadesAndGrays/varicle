#pragma once

#include "renderer/common/camera.hpp"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace varicle::renderer {

struct Transform {
    Vec3 position{};
    Vec3 scale{};
    Vec3 rotation{};

    glm::mat4 get_model_matrix() const;
};

} // namespace varicle::renderer

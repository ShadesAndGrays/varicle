#pragma once
#include <entt/entt.hpp>

namespace varicle {
class RenderSystem {
  public:
    /**
     * @brief Draws all entities with sprites and global transform components.
     * @param registry The EnTT registry containing active entities.
     */
    static void update_render_system(entt::registry& registry);
};

} // namespace varicle

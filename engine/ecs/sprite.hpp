#pragma once

#include "engine/core/color.hpp"
#include "transform.hpp"
#include <entt/entt.hpp>
#include <string>

namespace varicle::components {

struct Sprite {
    std::string texture_path = "";
    float       offset_x     = 0;
    float       offset_y     = 0;
    float       width        = 64;
    float       height       = 64;
    bool        flip_h       = false;
    bool        flip_v       = false;
    float       rotation     = 0;
};

/* require Sprite to hold texture_data. Cells Left to right, top to bottom*/
struct AnimatedSprite {
    float cell_width    = 64;
    float cell_height   = 64;
    int   h_cells       = 1;
    int   v_cells       = 1;
    int   total_frames  = 1;
    int   current_frame = 1;
};

struct Tint {
    varicle::Color tint = varicle::Color::White();
};

} // namespace varicle::components

namespace varicle {

class SpriteUtil {
  public:
    static void add_sprite_component(
        entt::registry&                    registry,
        entt::entity                       entity,
        const varicle::components::Sprite& sprite
    ) {
        registry.emplace_or_replace<varicle::components::Sprite>(entity) =
            sprite;

        registry.emplace_or_replace<varicle::components::GlobalTransform2D>(
            entity
        );

        registry.emplace_or_replace<varicle::components::LocalTransform2D>(
            entity
        );
    }

    static void add_animated_spirte_component(
        entt::registry&                            registry,
        entt::entity                               entity,
        const varicle::components::AnimatedSprite& animated_sprite
    ) {

        registry.emplace_or_replace<varicle::components::AnimatedSprite>(
            entity
        ) = animated_sprite;

        registry.emplace_or_replace<varicle::components::GlobalTransform2D>(
            entity
        );

        registry.emplace_or_replace<varicle::components::LocalTransform2D>(
            entity
        );
    }

    static void add_tint_component(
        entt::registry&                  registry,
        entt::entity                     entity,
        const varicle::components::Tint& tint
    ) {
        registry.emplace_or_replace<varicle::components::Tint>(entity) = tint;
    }
};

} // namespace varicle

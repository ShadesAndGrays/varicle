#include "engine/render/render-system.hpp"
// #include "engine/asset/raylib-asset.hpp"
// #include "engine/core/color.hpp"
// #include "engine/core/service-locator.hpp"
// #include "engine/ecs/components.hpp"
// #include "engine/util/sprite_util.hpp"

#include <entt/entt.hpp>
// #include <raylib.h>

void varicle::RenderSystem::update_render_system(entt::registry& registry) {
    // const auto view = registry.view<
    //     const components::Sprite,
    //     const components::GlobalTransform2D>();
    //
    // for (entt::entity entity : view) {
    //     const components::Sprite& sprite = view.get<components::Sprite>(entity);
    //
    //     const auto [global_position, global_scale, global_rotation] =
    //         view.get<components::GlobalTransform2D>(entity);
    //
    //     // TODO: Optimize to not use texture_path
    //     const auto texture = sprite.texture_path == ""
    //         ? nullptr
    //         : ServiceLocator::get<RaylibAssetLoader>().get_texture(
    //               sprite.texture_path
    //           );
    //
    //     const float width    = sprite.width * global_scale;
    //     const float height   = sprite.height * global_scale;
    //     const float rotation = sprite.rotation + (global_rotation * RAD2DEG);
    //
    //     ::Color raylibTint = texture
    //         ? ::WHITE
    //         : ::PURPLE; // set default color based on texture presence
    //
    //     // Check for color
    //     if (const components::Tint* tint_component =
    //             registry.try_get<components::Tint>(entity)) {
    //         raylibTint = ColorUtil::to_raylib_color(tint_component->tint);
    //     }
    //
    //     const Rectangle dest_rect = Rectangle{
    //         sprite.offset_x + global_position.x,
    //         sprite.offset_y + global_position.y,
    //         width,
    //         height,
    //     };
    //
    //     const Vector2 origin = Vector2{ width * 0.5f, height * 0.5f };
    //
    //     if (texture) {
    //         Rectangle src_rect;
    //         if (const components::AnimatedSprite* animated_sprite =
    //                 registry.try_get<components::AnimatedSprite>(entity)) {
    //             src_rect = get_source_rect(*animated_sprite);
    //         } else {
    //             src_rect = {
    //                 0.0f,
    //                 0.0f,
    //                 static_cast<float>(texture->width),
    //                 static_cast<float>(texture->height),
    //             };
    //         }
    //
    //         if (sprite.flip_h)
    //             src_rect.width *= -1;
    //         if (sprite.flip_v)
    //             src_rect.height *= -1;
    //
    //         DrawTexturePro(
    //             *(texture), src_rect, dest_rect, origin, rotation, raylibTint
    //         );
    //
    //     } else {
    //         DrawRectanglePro(dest_rect, origin, rotation, raylibTint);
    //     }
    // }
}

#pragma once

// #include "engine/ecs/components.hpp"
// #include <raylib.h>

namespace varicle {

// inline Rectangle
// get_source_rect(const varicle::components::AnimatedSprite& animated_sprite) {
//
//     if (animated_sprite.h_cells <= 0) {
//         return Rectangle{
//             0.0f, 0.0f, animated_sprite.cell_width, animated_sprite.cell_height
//         };
//     }
//
//     const float width  = animated_sprite.cell_width;
//     const float height = animated_sprite.cell_height;
//
//     const int frame   = animated_sprite.current_frame;
//     const int h_cells = animated_sprite.h_cells;
//
//     const int col = frame % h_cells;
//     const int row = frame / h_cells;
//
//     const float x = width * static_cast<float>(col);
//     const float y = height * static_cast<float>(row);
//
//     return { x, y, width, height };
// }

} // namespace varicle

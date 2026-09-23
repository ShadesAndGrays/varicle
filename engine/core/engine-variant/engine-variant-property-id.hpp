#pragma once

#include <entt/entt.hpp>

namespace varicle {
namespace PropertyID {
using namespace entt::literals;

// Guaranteed unique & typo-proof at compile time!

// LocalTransform Property IDs
constexpr auto TransformPosition = "transform:position"_hs;
constexpr auto TransformScale    = "transform:scale"_hs;
constexpr auto TransformRotation = "transform:rotation"_hs;

// GlobalTransform Property IDs
constexpr auto GlobalTransformPosition = "global_transform:position"_hs;
constexpr auto GlobalTransformScale    = "global_transform:scale"_hs;
constexpr auto GlobalTransformRotation = "global_transform:rotation"_hs;

// Physics Property IDs
constexpr auto Body2D_ID = "body_id"_hs;

// Sprite Property IDs
constexpr auto SpriteRotation = "sprite:rotation"_hs;
constexpr auto SpriteTexture  = "sprite:texture"_hs;
constexpr auto SpriteOffsetX  = "sprite:offset:x"_hs;
constexpr auto SpriteOffsetY  = "sprite:offset:y"_hs;
constexpr auto SpriteWidth    = "sprite:width"_hs;
constexpr auto SpriteHeight   = "sprite:height"_hs;
constexpr auto SpriteFlipV    = "sprite:flip_v"_hs;
constexpr auto SpriteFlipH    = "sprite:flip_h"_hs;

// AnimatedSprite Property IDs
constexpr auto AnimatedSpriteCellWidth    = "animated_sprite:cell_width"_hs;
constexpr auto AnimatedSpriteCellHeight   = "animated_sprite:cell_height"_hs;
constexpr auto AnimatedSpriteHCells       = "animated_sprite:h_cells"_hs;
constexpr auto AnimatedSpriteVCells       = "animated_sprite:v_cells"_hs;
constexpr auto AnimatedSpriteTotalFrames  = "animated_sprite:total_frames"_hs;
constexpr auto AnimatedSpriteCurrentFrame = "animated_sprite:current_frame"_hs;

// Tint Property IDs
constexpr auto TintColor = "tint:color"_hs;

// Parent Property IDs
constexpr auto Parent = "parent:parent"_hs;

// Children Property IDs
constexpr auto Children = "children:children"_hs;

} // namespace PropertyID

} // namespace varicle

#include "bindings.hpp"
#include "components.hpp"
#include "engine/core/engine-variant/engine-variant-property-id.hpp"
#include "engine/ecs/physics.hpp"

using namespace entt::literals;
using namespace varicle::components;

void varicle::register_all_component_properties(PropertyDatabase& db) {

    //----------------------------------------------------------
    // LocalTransform2D
    //----------------------------------------------------------
    db.register_property(
        PropertyID::TransformPosition,
        PropertyRegistry::bind_field(
            PropertyID::TransformPosition, &LocalTransform2D::position
        )
    );

    db.register_property(
        PropertyID::TransformScale,
        PropertyRegistry::bind_field(
            PropertyID::TransformScale, &LocalTransform2D::scale
        )
    );

    db.register_property(
        PropertyID::TransformRotation,
        PropertyRegistry::bind_field(
            PropertyID::TransformRotation, &LocalTransform2D::rotation
        )
    );

    //----------------------------------------------------------
    // GlobalTransform2D
    //----------------------------------------------------------
    db.register_property(
        PropertyID::GlobalTransformPosition,
        PropertyRegistry::bind_field(
            PropertyID::GlobalTransformPosition, &GlobalTransform2D::position
        )
    );

    db.register_property(
        PropertyID::GlobalTransformScale,
        PropertyRegistry::bind_field(
            PropertyID::GlobalTransformScale, &GlobalTransform2D::scale
        )
    );

    db.register_property(
        PropertyID::GlobalTransformRotation,
        PropertyRegistry::bind_field(
            PropertyID::GlobalTransformRotation, &GlobalTransform2D::rotation
        )
    );

    //----------------------------------------------------------
    // Physics
    //----------------------------------------------------------

    db.register_property(
        PropertyID::Body2D_ID,
        PropertyRegistry::bind_field(PropertyID::Body2D_ID, &Body2D::body_id)
    );

    //----------------------------------------------------------
    // Sprite
    //----------------------------------------------------------

    db.register_property(
        PropertyID::SpriteTexture,
        PropertyRegistry::bind_field(
            PropertyID::SpriteTexture, &Sprite::texture_path
        )
    );

    db.register_property(
        PropertyID::SpriteOffsetY,
        PropertyRegistry::bind_field(
            PropertyID::SpriteOffsetY, &Sprite::offset_y
        )
    );

    db.register_property(
        PropertyID::SpriteOffsetX,
        PropertyRegistry::bind_field(
            PropertyID::SpriteOffsetX, &Sprite::offset_x
        )
    );

    db.register_property(
        PropertyID::SpriteWidth,
        PropertyRegistry::bind_field(PropertyID::SpriteWidth, &Sprite::width)
    );

    db.register_property(
        PropertyID::SpriteHeight,
        PropertyRegistry::bind_field(PropertyID::SpriteHeight, &Sprite::height)
    );

    db.register_property(
        PropertyID::SpriteFlipH,
        PropertyRegistry::bind_field(PropertyID::SpriteFlipH, &Sprite::flip_h)
    );

    db.register_property(
        PropertyID::SpriteFlipV,
        PropertyRegistry::bind_field(PropertyID::SpriteFlipV, &Sprite::flip_v)
    );

    db.register_property(
        PropertyID::SpriteRotation,
        PropertyRegistry::bind_field(
            PropertyID::SpriteRotation, &Sprite::rotation
        )
    );

    //----------------------------------------------------------
    // AnimatedSprite
    //----------------------------------------------------------

    db.register_property(
        PropertyID::AnimatedSpriteCellWidth,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteCellWidth, &AnimatedSprite::cell_width
        )
    );

    db.register_property(
        PropertyID::AnimatedSpriteCellHeight,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteCellHeight, &AnimatedSprite::cell_height
        )
    );

    db.register_property(
        PropertyID::AnimatedSpriteHCells,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteHCells, &AnimatedSprite::h_cells
        )
    );

    db.register_property(
        PropertyID::AnimatedSpriteVCells,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteVCells, &AnimatedSprite::v_cells
        )
    );

    db.register_property(
        PropertyID::AnimatedSpriteTotalFrames,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteTotalFrames, &AnimatedSprite::total_frames
        )
    );

    db.register_property(
        PropertyID::AnimatedSpriteCurrentFrame,
        PropertyRegistry::bind_field(
            PropertyID::AnimatedSpriteCurrentFrame,
            &AnimatedSprite::current_frame
        )
    );

    //----------------------------------------------------------
    // Tint
    //----------------------------------------------------------

    db.register_property(
        PropertyID::TintColor,
        PropertyRegistry::bind_field(PropertyID::TintColor, &Tint::tint)
    );

    //----------------------------------------------------------
    // Parent
    //----------------------------------------------------------

    db.register_property(
        PropertyID::Parent,
        PropertyRegistry::bind_field(PropertyID::Parent, &Parent::parent)
    );

    //----------------------------------------------------------
    // Children
    //----------------------------------------------------------

    // db.register_property(PropertyID::Children,
    //                      PropertyRegistry::bind_field(PropertyID::Children,
    //                                                   &Children::children));
}

// TODO: implement property system for ui as well
void varicle::register_all_ui_properties(PropertyDatabase& db) {}

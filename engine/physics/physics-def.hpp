#pragma once

#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/util/slot-map.hpp"
#include <box2d/box2d.h>

namespace varicle::shape {

struct CircleShape {
    float radius;
};
struct RectangleShape {
    Vec2 half_extents;
};

using ShapeType = std::variant<std::monostate, CircleShape, RectangleShape>;

} // namespace varicle::shape

namespace varicle::physics {

using BodyID     = structures::SlotID;
using ShapeID    = structures::SlotID;
using ColliderID = structures::SlotID;

enum class BodyMode {
    STATIC,    // Unmoving
    KINEMATIC, // Program and Velocity based
    COLLIDABLE, // Program and Velocity based
    DYNAMIC,   // Simulated
    ZONE       // Detection (no collision)
};

struct EventBodyEntered {
    b2BodyId native_zone_body_id;
    b2BodyId body;
};

struct EventBodyExited {
    b2BodyId native_zone_body_id;
    b2BodyId body;
};

struct BodyData {

    b2BodyId native_body_id = {};

    ColliderID collider_id = {};

    BodyMode mode = BodyMode::STATIC;
};

struct ShapeData {
    b2ShapeId        native_shape_id = {};
    shape::ShapeType ShapeType       = std::monostate();
};

struct ColliderData {
    BodyID                         body_id = {};
    structures::SlotMap<ShapeData> native_shape_ids;
};

} // namespace varicle::physics

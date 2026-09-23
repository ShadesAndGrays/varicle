#include "physics-server.hpp"
#include "box2d/types.h"
#include "engine/core/event-bus.hpp"
#include "engine/core/service-locator.hpp"
// #include "raylib.h"

using namespace varicle::physics;
using namespace varicle::shape;

PhysicsServer2D::PhysicsServer2D() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity    = (b2Vec2){ 0.0f, -10.0f };
    m_world_id          = b2CreateWorld(&worldDef);
}
varicle::physics::PhysicsServer2D::~PhysicsServer2D() {
    b2DestroyWorld(m_world_id);
};

void varicle::physics::PhysicsServer2D::debug_draw_colliders() {

    const auto& colliders = m_colliders.get_data();

    for (const auto& collider : colliders) {
        const auto& shapes = collider.native_shape_ids.get_data();
        const auto  pos    = get_position(collider.body_id);
        const auto  rot    = get_rotation(collider.body_id);
        for (const auto& shape : shapes) {
            std::visit(
                [&](const auto& shape) {
                    using T = std::decay_t<decltype(shape)>;
                    if constexpr (std::is_same_v<T, CircleShape>) {
                        // DrawCircle(
                        //     static_cast<int>(pos.x),
                        //     static_cast<int>(pos.y),
                        //     shape.radius,
                        //     m_debug_collision_color
                        // );
                    } else if constexpr (std::is_same_v<T, RectangleShape>) {

                    //     DrawRectanglePro(
                    //         { pos.x,
                    //           pos.y,
                    //           shape.half_extents.x * 2,
                    //           shape.half_extents.y * 2 },
                    //         Vector2{ shape.half_extents.x,
                    //                  shape.half_extents.y },
                    //         rot * RAD2DEG,
                    //         m_debug_collision_color
                    //
                    //     );
                    // } else {
                    //     DrawCircle(
                    //         static_cast<int>(pos.x),
                    //         static_cast<int>(pos.y),
                    //         2,
                    //         ::RED
                    //     );
                    };
                },
                shape.ShapeType
            );
        }
    }
}

// Collider
ColliderID PhysicsServer2D::create_collider(BodyID body_id) {
    const auto collider = m_colliders.push({ body_id });
    return collider;
}
const ColliderData*
PhysicsServer2D::get_collider(ColliderID collider_id) const {
    return m_colliders.get(collider_id);
}

void PhysicsServer2D::destroy_collider(ColliderID collider_id) {
    m_colliders.remove(collider_id);
}

ShapeID
PhysicsServer2D::add_shape(ColliderID collider_id, ShapeType shape_type) {
    auto       collider       = m_colliders.get(collider_id);
    auto const body_data      = m_bodies.get(collider->body_id);
    const auto native_body_id = body_data->native_body_id;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    // shapeDef.density = 10.0f;
    shapeDef.density            = 10.0f;
    shapeDef.material.friction  = 0.7f;
    shapeDef.enableSensorEvents = true;

    if (body_data->mode == BodyMode::ZONE) {
        shapeDef.isSensor = true;
    }

    b2ShapeId native_shape_id;

    std::visit(
        [native_body_id, &native_shape_id, &shapeDef](const auto& s) {
            using T = std::decay_t<decltype(s)>;

            if constexpr (std::is_same_v<T, CircleShape>) {
                b2Circle circle;
                circle.center = { .0f, .0f };
                circle.radius = pixels_to_meters(s.radius);
                native_shape_id =
                    b2CreateCircleShape(native_body_id, &shapeDef, &circle);
            } else if constexpr (std::is_same_v<T, RectangleShape>) {
                const auto x           = pixels_to_meters(s.half_extents.x);
                const auto y           = pixels_to_meters(s.half_extents.y);
                b2Polygon  box_polygon = b2MakeBox(x, y);
                native_shape_id        = b2CreatePolygonShape(
                    native_body_id, &shapeDef, &box_polygon
                );
            }
        },
        shape_type
    );

    return collider->native_shape_ids.push({ native_shape_id, shape_type });
}

void PhysicsServer2D::remove_shape(ColliderID collider_id, ShapeID shape_id) {

    if (auto collider = m_colliders.get(collider_id)) {
        const auto shape_data = collider->native_shape_ids.get(shape_id);
        b2DestroyShape(shape_data->native_shape_id, false);
        collider->native_shape_ids.remove(shape_id);
    }
}

// Body
BodyID PhysicsServer2D::create_body(BodyMode mode, Vec2 position) {

    b2BodyDef bodyDef = b2DefaultBodyDef();
    switch (mode) {
    case BodyMode::KINEMATIC:
        bodyDef.type = b2_kinematicBody;
        break;
    case BodyMode::COLLIDABLE:
        bodyDef.type = b2_kinematicBody;
        break;
    case BodyMode::STATIC:
        bodyDef.type = b2_staticBody;
        break;
    case BodyMode::ZONE:
        bodyDef.type = b2_staticBody;
        break;
    case BodyMode::DYNAMIC:
        bodyDef.type = b2_dynamicBody;
        break;
    }

    bodyDef.position = to_b2(position);

    const b2BodyId native_body_id = b2CreateBody(m_world_id, &bodyDef);

    BodyData body;
    body.mode            = mode;
    body.native_body_id  = native_body_id;
    const BodyID body_id = m_bodies.emplace(body);

    const ColliderID collier_id    = create_collider(body_id);
    get_body(body_id)->collider_id = collier_id;
    return body_id;
}

BodyData* PhysicsServer2D::get_body(BodyID body_id) {
    return m_bodies.get(body_id);
}
const BodyData* PhysicsServer2D::get_body(BodyID body_id) const {
    return m_bodies.get(body_id);
}

void PhysicsServer2D::destroy_body(BodyID body_id) {
    const auto body = get_body(body_id);
    b2DestroyBody(body->native_body_id);
    m_bodies.remove(body_id);
}

void PhysicsServer2D::set_position(BodyID body_id, Vec2 position) {
    if (const auto body = get_body(body_id)) {
        auto rot = b2Body_GetRotation(body->native_body_id);
        b2Body_SetTransform(body->native_body_id, to_b2(position), rot);
    }
}
varicle::Vec2 PhysicsServer2D::get_position(BodyID body_id) {
    if (const auto body = get_body(body_id)) {
        const auto position = b2Body_GetPosition(body->native_body_id);
        return to_pixels(position);
    }
    return {};
}

void PhysicsServer2D::set_rotation(BodyID body_id, float rotation) {
    if (const auto body = get_body(body_id)) {
        auto pos = b2Body_GetPosition(body->native_body_id);
        b2Body_SetTransform(body->native_body_id, pos, to_b2_rot(rotation));
    }
}
float PhysicsServer2D::get_rotation(BodyID body_id) {
    const auto body = get_body(body_id);
    auto       rot  = b2Body_GetRotation(body->native_body_id);
    return to_rad(rot);
}

void PhysicsServer2D::set_velocity(BodyID body_id, Vec2 velocity) {
    if (const auto body = get_body(body_id)) {
        if (!b2Body_IsValid(body->native_body_id))
            return;
        b2Body_SetLinearVelocity(body->native_body_id, to_b2(velocity));
    }
}

varicle::Vec2 PhysicsServer2D::get_velocity(BodyID body_id) {
    if (const auto body = get_body(body_id)) {
        const auto velocity = b2Body_GetLinearVelocity(body->native_body_id);
        return to_pixels(velocity);
    }
    return {};
}

// Processing

void PhysicsServer2D::step(float dt) {
    b2World_Step(m_world_id, time_step, sub_step_count);
    handle_sensor_events();
}

void PhysicsServer2D::handle_sensor_events() {
    b2SensorEvents sensor_events = b2World_GetSensorEvents(m_world_id);

    for (int i = 0; i < sensor_events.beginCount; ++i) {
        b2SensorBeginTouchEvent* begin_touch = sensor_events.beginEvents + i;

        ServiceLocator::get<event::EventBus>().publish<EventBodyEntered>(
            { b2Shape_GetBody(begin_touch->sensorShapeId) }
        );
    }

    for (int i = 0; i < sensor_events.endCount; ++i) {
        b2SensorEndTouchEvent* end_touch = sensor_events.endEvents + i;

        ServiceLocator::get<event::EventBus>().publish<EventBodyExited>(
            { b2Shape_GetBody(end_touch->sensorShapeId) }
        );
    }
}

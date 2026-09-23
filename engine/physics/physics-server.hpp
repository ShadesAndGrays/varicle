#pragma once

#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/util/slot-map.hpp"
#include "physics-def.hpp"
#include <box2d/box2d.h>

namespace varicle::physics {

class PhysicsServer2D {
  private:
    Color   m_debug_collision_color = Color({ 227, 86, 67, 50 });
    b2WorldId m_world_id;
    structures::SlotMap<BodyData>     m_bodies;
    structures::SlotMap<ColliderData> m_colliders;
    structures::SlotMap<entt::entity> m_body_to_entity;

    static constexpr float time_step      = 1.0f / 60.0f;
    static constexpr float sub_step_count = 4;
    static constexpr float PPM            = 32.0f;

  public:
    bool debug = false;

  private:
    void handle_sensor_events();

  public:
    PhysicsServer2D();
    ~PhysicsServer2D();

    void debug_draw_colliders();

    static constexpr float pixels_to_meters(float px) { return px / PPM; }
    static constexpr float meters_to_pixels(float m) { return m * PPM; }

    static b2Vec2 to_b2(Vec2 px_vec) {
        return b2Vec2{ px_vec.x / PPM, (px_vec.y / PPM) * -1.0f };
    }

    static Vec2 to_pixels(b2Vec2 m_vec) {
        return Vec2{ m_vec.x * PPM, m_vec.y * PPM * -1.0f };
    }

    static b2Rot to_b2_rot(float radians) { return b2MakeRot(-radians); }
    static float to_rad(b2Rot rot) {
        rot.s *= -1;
        return b2Rot_GetAngle(rot);
    }

    // Collider
    ColliderID          create_collider(BodyID body_id);
    const ColliderData* get_collider(ColliderID collider_id) const;
    void                destroy_collider(ColliderID collider_id);

    ShapeID add_shape(ColliderID collider_id, shape::ShapeType shape_type);
    void    remove_shape(ColliderID collider_id, ShapeID shape_id);

    // Body
    BodyID create_body(
        BodyMode mode             = BodyMode::STATIC,
        Vec2     initial_position = { 0.0f, 0.0f }
    );
    const BodyData* get_body(BodyID body_id) const;
    BodyData*       get_body(BodyID body_id);
    void            destroy_body(BodyID body_id);

    void set_position(BodyID body_id, Vec2 position);
    Vec2 get_position(BodyID body_id);

    void  set_rotation(BodyID body_id, float rotation);
    float get_rotation(BodyID body_id);

    void set_velocity(BodyID body_id, Vec2 velocity);
    Vec2 get_velocity(BodyID body_id);

    void step(float dt);
};

} // namespace varicle::physics

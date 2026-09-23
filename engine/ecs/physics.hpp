#pragma once
#include "engine/core/service-locator.hpp"
#include "engine/ecs/transform.hpp"
#include "engine/physics/physics-server.hpp"

namespace varicle::components {

struct Body2D {
    physics::BodyID body_id;
};

} // namespace varicle::components
namespace varicle {

class PhysicsSystem {
  public:
    // Updates LocalTransform Should be called before transform sync
    static void update_physics_system(entt::registry& registry, float dt) {
        using namespace components;

        auto physic_group = registry.view<const Body2D, LocalTransform2D>();

        physic_group.each(
            [dt](const Body2D& body_component, LocalTransform2D& transform) {
                auto& physics_server_2d =
                    ServiceLocator::get<physics::PhysicsServer2D>();
                transform.position =
                    physics_server_2d.get_position(body_component.body_id);
                transform.rotation =
                    physics_server_2d.get_rotation(body_component.body_id);
            }
        );
    }
};

class PhysicsUtil {
  private:
    static void add_body_component(
        entt::registry&   registry,
        entt::entity      e,
        physics::BodyMode mode
    ) {

        auto& physics_server_2d =
            ServiceLocator::get<physics::PhysicsServer2D>();
        Vec2 initial_position = { .0f, .0f };

        if (const auto transform =
                registry.try_get<components::GlobalTransform2D>(e)) {
            initial_position = transform->position;
        };
        auto& body_2d   = registry.emplace<components::Body2D>(e);
        body_2d.body_id = physics_server_2d.create_body(mode, initial_position);
    }

  public:
    static physics::BodyID
    get_body_id(entt::registry& registry, entt::entity e) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            return body_component->body_id;
        }
        return { .index = std::numeric_limits<uint32_t>().max() };
    }

    static physics::BodyData*
    get_body_data(entt::registry& registry, entt::entity e) {
        auto& physics_server_2d =
            ServiceLocator::get<physics::PhysicsServer2D>();
        return physics_server_2d.get_body(get_body_id(registry, e));
        return nullptr;
    }

    static void
    set_velocity(entt::registry& registry, entt::entity e, Vec2 velocity) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            auto& physics_server_2d =
                ServiceLocator::get<physics::PhysicsServer2D>();
            physics_server_2d.set_velocity(body_component->body_id, velocity);
        }
    }
    static Vec2
    get_velocity(entt::registry& registry, entt::entity e) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            auto& physics_server_2d =
                ServiceLocator::get<physics::PhysicsServer2D>();
            return physics_server_2d.get_velocity(body_component->body_id);
        }
        std::unreachable();
    }

    static void
    set_position(entt::registry& registry, entt::entity e, Vec2 position) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            auto& physics_server_2d =
                ServiceLocator::get<physics::PhysicsServer2D>();
            physics_server_2d.set_position(body_component->body_id, position);
        }
    }

    static Vec2 get_position(entt::registry& registry, entt::entity e) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            auto& physics_server_2d =
                ServiceLocator::get<physics::PhysicsServer2D>();
            return physics_server_2d.get_position(body_component->body_id);
        }
        std::unreachable();
    }

    static bool is_entity_body(entt::registry& registry, entt::entity e,b2BodyId body) {
        if (auto data = get_body_data(registry,e)) {
            return B2_ID_EQUALS(data->native_body_id,body);
        }
        return false;
    }


    static void
    set_rotation(entt::registry& registry, entt::entity e, float rotation) {
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            auto& physics_server_2d =
                ServiceLocator::get<physics::PhysicsServer2D>();
            physics_server_2d.set_rotation(body_component->body_id, rotation);
        }
    }

    static void
    add_static_body_component(entt::registry& registry, entt::entity e) {
        add_body_component(registry, e, physics::BodyMode::STATIC);
    }

    static void
    add_kinematic_body_component(entt::registry& registry, entt::entity e) {
        add_body_component(registry, e, physics::BodyMode::KINEMATIC);
    }

    static void
    add_dynamic_body_component(entt::registry& registry, entt::entity e) {
        add_body_component(registry, e, physics::BodyMode::DYNAMIC);
    }

    static void
    add_zone_body_component(entt::registry& registry, entt::entity e) {
        add_body_component(registry, e, physics::BodyMode::ZONE);
    }

    static void add_rect_collider(
        entt::registry& registry,
        entt::entity    e,
        Vec2            half_extents
    ) {
        auto& physics_server_2d =
            ServiceLocator::get<physics::PhysicsServer2D>();
        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {
            const auto body =
                physics_server_2d.get_body(body_component->body_id);
            physics_server_2d.add_shape(
                body->collider_id, shape::RectangleShape{ half_extents }
            );

        } else {
            std::cout
                << "Warning: Cannot add collider to entity without body2D\n";
        }
    }

    static void add_circle_collider(
        entt::registry& registry,
        entt::entity    e,
        float           radius
    ) {
        auto& physics_server_2d =
            ServiceLocator::get<physics::PhysicsServer2D>();

        if (const auto body_component =
                registry.try_get<components::Body2D>(e)) {

            const auto body =
                physics_server_2d.get_body(body_component->body_id);
            physics_server_2d.add_shape(
                body->collider_id, shape::CircleShape{ radius }
            );
        }
    }
};
} // namespace varicle

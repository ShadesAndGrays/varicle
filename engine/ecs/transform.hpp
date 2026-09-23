#pragma once

#include "engine/core/engine-variant/engine-variant-operation.hpp"
#include "engine/core/engine-variant/engine-variant-property-id.hpp"
#include "engine/core/engine-variant/engine-variant-property.hpp"
#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/core/service-locator.hpp"
#include "hierachy.hpp"
#include <cmath>
#include <entt/entt.hpp>
#include <unordered_map>

namespace varicle::components {

struct LocalTransform2D {
    Vec2  position = { 0, 0 };
    float scale    = 1;
    float rotation = 0;
};

struct GlobalTransform2D {
    Vec2  position = { 0, 0 };
    float scale    = 1;
    float rotation = 0;
};

} // namespace varicle::components

namespace varicle {

class TransformUtil {
  public:
    static void
    add_transform_components(entt::registry& registry, entt::entity entity) {

        auto g = registry.get_or_emplace<components::GlobalTransform2D>(entity);
        auto l = registry.get_or_emplace<components::LocalTransform2D>(entity);
    }

    static void set_position(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_position
    ) {
        auto& pd = ServiceLocator::get<PropertyDatabase>();
        if (auto prop = p_position.try_get<Vec2>())
            pd.set_value(
                registry, entity, PropertyID::TransformPosition, *prop
            );
    }
    static void set_scale(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_scale
    ) {
        auto& pd = ServiceLocator::get<PropertyDatabase>();
        if (auto prop = p_scale.try_get<float>())
            pd.set_value(registry, entity, PropertyID::TransformScale, *prop);
    }
    static void set_rotation(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_rotation
    ) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        if (auto prop = p_rotation.try_get<float>())
            pd.set_value(
                registry, entity, PropertyID::TransformRotation, *prop
            );
    }
    static EngineVariant
    get_position(entt::registry& registry, entt::entity entity) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(registry, entity, PropertyID::TransformPosition);
    }
    static EngineVariant
    get_scale(entt::registry& registry, entt::entity entity) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(registry, entity, PropertyID::TransformScale);
    }
    static EngineVariant
    get_rotation(entt::registry& registry, entt::entity entity) {
        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(registry, entity, PropertyID::TransformRotation);
    }

    static void set_global_position(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_position
    ) {
        PropertyDatabase& pd = ServiceLocator::get<PropertyDatabase>();

        EngineVariant current_local_position =
            pd.get_value(registry, entity, PropertyID::TransformPosition);

        if (auto prop = p_position.try_get<Vec2>())
            pd.set_value(registry, entity, PropertyID::TransformPosition, prop);
    }
    static void set_global_scale(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_scale
    ) {
        auto& pd = ServiceLocator::get<PropertyDatabase>();
        if (auto prop = p_scale.try_get<float>())
            pd.set_value(registry, entity, PropertyID::TransformScale, prop);
    }
    static void set_global_rotation(
        entt::registry& registry,
        entt::entity    entity,
        EngineVariant   p_rotation
    ) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        if (auto prop = p_rotation.try_get<float>())
            pd.set_value(registry, entity, PropertyID::TransformRotation, prop);
    }

    static EngineVariant
    get_global_position(entt::registry& registry, entt::entity entity) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(
            registry, entity, PropertyID::GlobalTransformPosition
        );
    }
    static EngineVariant
    get_global_scale(entt::registry& registry, entt::entity entity) {

        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(registry, entity, PropertyID::GlobalTransformScale);
    }
    static EngineVariant
    get_global_rotation(entt::registry& registry, entt::entity entity) {
        auto& pd = ServiceLocator::get<PropertyDatabase>();
        return pd.get_value(
            registry, entity, PropertyID::GlobalTransformRotation
        );
    }
};

class TransformSystem {

  public:
    static void update_global_transform(entt::registry& registry) {

        using namespace varicle::components;

        auto view = registry.view<GlobalTransform2D, const LocalTransform2D>(
            entt::exclude<Parent>
        );

        view.each([&](entt::entity            entity,
                      GlobalTransform2D&      gt,
                      const LocalTransform2D& lt) {
            gt.position = lt.position;
            gt.rotation = lt.rotation;
            gt.scale    = lt.scale;

            update_children_global_transform(registry, entity);
        });
    }

  private:
    static void update_global_position(
        entt::registry& registry,
        entt::entity    parent,
        entt::entity    entity
    ) {
        EngineVariant parent_global_position =
            TransformUtil::get_global_position(registry, parent);

        EngineVariant parent_global_scale =
            TransformUtil::get_global_scale(registry, parent);

        EngineVariant parent_global_rotation =
            TransformUtil::get_global_rotation(registry, parent);

        EngineVariant local_position =
            TransformUtil::get_position(registry, entity);

        // scale local position
        // VariantOpManager::ExecuteOperation({.target = &local_position,
        //                                     .operation = OpType::Multiply,
        //                                     .operand = parent_global_scale});

        local_position = VariantOpManager::Execute(
            { local_position, op::Mul{ parent_global_scale } }
        );

        float angle   = parent_global_rotation.get<float>();
        float local_x = local_position.get<Vec2>().x;
        float local_y = local_position.get<Vec2>().y;

        float rotated_local_x = local_x * cos(angle) - local_y * sin(angle);
        float rotated_local_y = local_x * sin(angle) + local_y * cos(angle);

        local_position = VariantOpManager::Execute(
            { local_position,
              op::Assign{ Vec2{ rotated_local_x, rotated_local_y } } }
        );

        parent_global_position = VariantOpManager::Execute(
            { parent_global_position, op::Add{ local_position } }
        );

        registry.get<components::GlobalTransform2D>(entity).position =
            parent_global_position.get<Vec2>(); // force set global transform
    }

    static void update_global_scale(
        entt::registry& registry,
        entt::entity    parent,
        entt::entity    entity
    ) {
        EngineVariant parent_global_scale =
            TransformUtil::get_global_scale(registry, parent);

        EngineVariant local_scale = TransformUtil::get_scale(registry, entity);

        parent_global_scale = VariantOpManager::Execute(
            { .source    = parent_global_scale,
              .operation = op::Mul{ local_scale } }
        );

        registry.get<components::GlobalTransform2D>(entity).scale =
            parent_global_scale.get<float>(); // force set global transform
    }

    static void update_global_rotation(
        entt::registry& registry,
        entt::entity    parent,
        entt::entity    entity
    ) {
        EngineVariant parent_global_rotation =
            TransformUtil::get_global_rotation(registry, parent);

        EngineVariant local_rotation =
            TransformUtil::get_rotation(registry, entity);

        // add the two rotations

        parent_global_rotation = VariantOpManager::Execute(
            { .source    = parent_global_rotation,
              .operation = op::Add{ local_rotation } }
        );

        registry.get<components::GlobalTransform2D>(entity).rotation =
            parent_global_rotation.get<float>(); // force set global transform
    }

    static void update_children_global_transform(
        entt::registry& registry,
        entt::entity    entity
    ) {

        if (entity == entt::null)
            return;

        std::vector<entt::entity> stack;
        stack.push_back(entity);

        std::unordered_map<entt::entity, bool> visited; // just for safety check

        while (!stack.empty()) {
            auto current = stack.back();
            stack.pop_back();

            // do the thing
            auto parent = Hierachy::get_parent(registry, current);
            if (parent != entt::null) {
                update_global_position(registry, parent, current);
                update_global_rotation(registry, parent, current);
                update_global_scale(registry, parent, current);
            }

            if (auto* children_component =
                    registry.try_get<varicle::components::Children>(current)) {

                for (auto child : children_component->children) {
                    if (visited.contains(child) && visited.at(child)) {
                        continue;
                    } // safety check

                    if (!registry.all_of<
                            varicle::components::GlobalTransform2D,
                            varicle::components::LocalTransform2D>(child))
                        return;

                    visited[child] = true;
                    stack.push_back(child);
                }
            }
        }
    }
};
} // namespace varicle

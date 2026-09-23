#pragma once
#include <entt/entt.hpp>

namespace varicle::components {

struct Parent {
    entt::entity parent = entt::null;
};

// enables top down traversal
struct Children {
    std::vector<entt::entity> children;
};

} // namespace varicle::components

namespace varicle {

class Hierachy {
  public:
    static void set_parent(
        entt::registry& registry,
        entt::entity    child,
        entt::entity    parent
    ) {
        remove_parent(registry, child);

        if (parent == entt::null)
            return;

        registry.emplace_or_replace<components::Parent>(child, parent);

        auto& children_component =
            registry.get_or_emplace<components::Children>(parent);
        children_component.children.push_back(child);
    }

    static void remove_parent(entt::registry& registry, entt::entity child) {
        components::Parent* parent_component =
            registry.try_get<components::Parent>(child);

        if (!parent_component ||
            parent_component->parent == entt::null) // if no parent return
            return;

        entt::entity previous_parent = parent_component->parent;

        if (auto* children_component =
                registry.try_get<components::Children>(previous_parent)) {
            auto& children = children_component->children;
            std::erase(children, child);

            if (children.empty()) { // if no more children
                registry.remove<components::Children>(previous_parent);
            }
        }

        registry.remove<components::Parent>(child);
    }

    static void destroy_entity(entt::registry& registry, entt::entity entity) {
        if (auto* children_component =
                registry.try_get<components::Children>(entity)) {
            auto children_copy = children_component->children;
            for (auto child : children_copy) {
                destroy_entity(registry, child);
            }
        }

        remove_parent(registry, entity);

        registry.destroy(entity);
    }

    static entt::entity
    get_parent(entt::registry& registry, entt::entity entity) {

        if (auto* parent_component =
                registry.try_get<components::Parent>(entity)) {
            return parent_component->parent;
        }

        return entt::null;
    }
    static std::vector<entt::entity>
    get_children(entt::registry& registry, entt::entity entity) {
        if (auto* children_component =
                registry.try_get<components::Children>(entity)) {
            return children_component->children;
        }
        return {};
    }
};

} // namespace varicle

#include "tween.hpp"
#include "engine/core/engine-variant/engine-variant.hpp"
#include <cstdint>
#include <entt/entt.hpp>

namespace v = varicle;

v::Tween& v::TweenManager::create_tween(
    entt::entity  p_entity,
    std::uint32_t p_target,
    EngineVariant p_start,
    EngineVariant p_end,
    float         p_duration
) {
    return active_tweens.emplace_back(
        std::move(
            Tween{
                .entity   = p_entity,
                .target   = p_target,
                .start    = p_start,
                .end      = p_end,
                .duration = p_duration,
            }
        )
    );
}

void varicle::TweenManager::update(entt::registry& registry, float dt) {
    const auto& pd = ServiceLocator::get<PropertyDatabase>();

    for (Tween& tween : active_tweens) {

        switch (tween.state) {
        case TweenState::PLAYING:
            if (!registry.valid(tween.entity)) {
                tween.state = TweenState::DEAD;
                break;
            }

            if (tween.play_time < tween.duration) {
                tween.play_time += dt;

                float progress =
                    std::min(tween.play_time / tween.duration, 1.0f);
                auto current = VariantOpManager::Execute(
                    { tween.start,
                      op::Lerp{ tween.end, progress, tween.curve, tween.ease } }
                );

                pd.set_value(registry, tween.entity, tween.target, current);
            } else {
                tween.state = TweenState::FINISHED;
            }
            break;
        case TweenState::PAUSED:
            break;
        case TweenState::FINISHED:
            if (registry.valid(tween.entity))
                pd.set_value(registry, tween.entity, tween.target, tween.end);
            tween.state = TweenState::DEAD;
            break;
        case TweenState::DEAD:
            break;
        }
    }

    // Clean up
    std::erase_if(active_tweens, [](const Tween& t) {
        return t.state == TweenState::DEAD;
    });
}

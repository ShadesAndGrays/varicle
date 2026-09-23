#pragma once
#include "engine/core/engine-variant/engine-variant-operation.hpp"
#include "engine/core/engine-variant/engine-variant-property.hpp"
#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/core/lerp.hpp"
#include "engine/core/service-locator.hpp"

#include <entt/entt.hpp>
#include <vector>

namespace varicle {

enum TweenState {
    PLAYING,
    FINISHED,
    PAUSED,
    DEAD,
};

struct Tween {

    entt::entity  entity = entt::null;
    std::uint32_t target;

    EngineVariant start;
    EngineVariant end;
    float         duration = 1.0f;

    float     play_time = 0.0f;
    CurveFunc curve     = Curve::linear;
    EaseMode  ease      = EaseMode::In;

    TweenState state = PLAYING;

    void pause_tween() { state = PAUSED; }
    void play_tween() { state = PLAYING; }

    Tween& set_ease(EaseMode p_ease) {
        ease = p_ease;
        return *this;
    }
    Tween& set_curve(CurveFunc p_curve) {
        curve = p_curve;
        return *this;
    }
    Tween& set_duration(float p_duration) {
        duration = p_duration;
        return *this;
    }
};

class TweenManager {
    std::vector<Tween> active_tweens;

  public:
    Tween& create_tween(
        entt::entity  p_entity,
        std::uint32_t p_target,
        EngineVariant p_start,
        EngineVariant p_end,
        float         p_duration
    );

    void update(entt::registry& registry, float dt);
};
} // namespace varicle

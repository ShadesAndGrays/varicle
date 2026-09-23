#include "animation.hpp"

#include "engine/core/engine-variant/engine-variant-operation.hpp"
#include "engine/core/engine-variant/engine-variant-property.hpp"
#include "engine/core/service-locator.hpp"
#include "engine/ecs/animation.hpp"
#include <algorithm>
#include <cstdint>
#include <entt/entt.hpp>
// #include <format>

using namespace varicle;

// Animation Manager
void AnimationManager::add_animation(const std::string& animation_name) {
    AnimationData data;
    data.animation_name = animation_name;
    const uint32_t hashed_name =
        entt::hashed_string{ animation_name.c_str() }.value();
    data.animation_name_hs = hashed_name;

    if (!animations.contains(hashed_name)) {
        animations[hashed_name] = data;
    }
}

void AnimationManager::set_animation_duration(
    std::uint32_t animation_name_hs,
    float         duration
) {
    animations[animation_name_hs].duration = duration;
}

AnimationData AnimationManager::get_animation(std::uint32_t animation_name_hs) {
    if (!animations.contains(animation_name_hs)) {
        return {};
    }
    return animations[animation_name_hs];
}

void AnimationManager::add_track(std::uint32_t animation_name_hs, Track track) {
    if (!animations.contains(animation_name_hs)) {
        return;
    }

    animations[animation_name_hs].tracks[track.target] = track;
}

// Animation System
void AnimationSystem::update_animation_system(
    entt::registry& registry,
    float           dt
) {

    const auto animatable = registry.view<Animator>();

    animatable.each([&registry, dt](entt::entity entity, Animator& animator) {
        if (!animator.playing)
            return;

        const float duration = AnimationUtil::get_animation_duration(animator);

        // progress animation

        animator.play_time += dt * animator.play_speed;

        if (animator.play_time > duration) {
            if (animator.looping) {
                animator.play_time -= duration;
            } else {
                animator.playing = false;
            }
        }
        animator.play_time = std::clamp(animator.play_time, 0.0f, duration);

        const float current_time = animator.reverse
            ? duration - animator.play_time
            : animator.play_time;

        auto& pd   = ServiceLocator::get<PropertyDatabase>();
        auto  anim = AnimationUtil::get_animation(animator);

        for (const auto& track : anim.tracks) {
            const auto keys   = track.second.keys; // copy of keys
            const auto target = track.second.target;

            if (current_time <= 0) { // lower bound
                pd.set_value(registry, entity, target, keys.front().value);
            } else if (current_time >= keys.back().timeframe) { // upper bound
                pd.set_value(registry, entity, target, keys.back().value);
            } else { // in-between

                for (int i = 0; i < keys.size() - 1; i++) {
                    const auto a = keys[i];
                    const auto b = keys[i + 1];

                    if (current_time > b.timeframe ||
                        current_time < a.timeframe)
                        continue;

                    const float alpha = (b.timeframe - current_time) /
                        (b.timeframe - a.timeframe);

                    auto final_value = VariantOpManager::Execute(
                        {
                            a.value,
                            op::Lerp{ b.value,
                                      alpha,
                                      track.second.interpolation,
                                      track.second.mode },
                        }
                    );

                    pd.set_value(registry, entity, target, final_value);
                };
            }
        }
    });
}

// Animation Utilities

void AnimationUtil::add_animator_component(
    entt ::registry& registry,
    entt::entity     e,
    const Animator&  animator
) {
    registry.emplace<Animator>(e) = animator;
}

AnimationData AnimationUtil::get_animation(Animator& animator) {
    auto anim = ServiceLocator::get<AnimationManager>().get_animation(
        animator.animation_name
    );
    return anim;
}

float AnimationUtil::get_animation_duration(Animator& animator) {
    if (ServiceLocator::has<AnimationManager>()) {
        auto animation_data = get_animation(animator);
        return animation_data.duration;
    } else {
        return 0.0;
    }
}

float AnimationUtil::get_animation_duration(AnimationData& animation) {
    return animation.duration;
}

void AnimationUtil::play_animation(Animator& animator, bool reset) {
    animator.playing = true;
    if (reset)
        animator.play_time = 0.0f;
}

void AnimationUtil::play_animation(
    Animator&     animator,
    std::uint32_t animation_name_hs,
    bool          reset
) {
    animator.animation_name = animation_name_hs;
    play_animation(animator, reset);
}

void AnimationUtil::stop_animation(Animator& animator, bool reset) {
    animator.playing = false;
    if (reset) {
        animator.play_time = 0.0f;
    }
}

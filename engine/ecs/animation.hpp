#pragma once

#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/core/lerp.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace varicle {

struct Key {
    float         timeframe;
    EngineVariant value;
};

struct Track {
    // key
    CurveFunc interpolation = Curve::linear; // key to key sampling
    EaseMode  mode          = EaseMode::In;

    std::vector<Key> keys; // keyframes

    std::uint32_t target; // property hash
};

struct AnimationData {
    std::string   animation_name;                    // name of animaton
    std::uint32_t animation_name_hs;                 // hash of animation name
    std::unordered_map<std::uint32_t, Track> tracks; // tracks
    float                                    duration;
};

struct Animator {
    std::uint32_t animation_name;     // name of animation
    float         play_time  = 0.0f;  // current time to sample
    bool          playing    = false; // is animation currently being played
    bool          looping    = false; // Should animation repeat after ending
    bool          reverse    = false; //    should play in reverse
    float         play_speed = 1.f;   // Speed of playback
};

class AnimationManager {
  private:
    std::unordered_map<std::uint32_t, AnimationData> animations;

  public:
    void add_animation(const std::string& animation_name);
    void
    set_animation_duration(std::uint32_t animation_name_hs, float duration);
    AnimationData get_animation(std::uint32_t animation_name_hs);
    void          add_track(std::uint32_t animation_name_hs, Track track);
};

class AnimationSystem {
  public:
    static void update_animation_system(entt::registry& registry, float dt);
};

class AnimationUtil {
  public:
    static void add_animator_component(
        entt ::registry& registry,
        entt::entity     e,
        const Animator&  animator
    );
    static AnimationData get_animation(Animator& animator);
    static float         get_animation_duration(Animator& animator);
    static float         get_animation_duration(AnimationData& animation);

    static void play_animation(Animator& animator, bool reset = false);
    static void play_animation(
        Animator&     animator,
        std::uint32_t animation_name_hs,
        bool          reset = false
    );
    static void stop_animation(Animator& animator, bool reset = false);
};

} // namespace varicle

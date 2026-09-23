#pragma once

#include "engine/core/constants.hpp"
#include <algorithm>
#include <cmath>
#include <functional>

namespace varicle {

using CurveFunc = std::function<float(float)>;

enum class EaseMode { In, Out, InOut, OutIn };

class Curve {
  private:
    static inline float clamp01(float t) { return std::clamp(t, 0.0f, 1.0f); }

  public:
    static float linear(float t) { return clamp01(t); }

    static float quad(float t) {
        t = clamp01(t);
        return t * t;
    }

    static float cubic(float t) {
        t = clamp01(t);
        return t * t * t;
    }

    static float smoothstep(float t) {
        t = clamp01(t);
        return t * t * (3.0f - 2.0f * t);
    }

    static float exponential(float t, float k = 10.0f) {
        t = clamp01(t);

        if (std::abs(k) < 0.0001f)
            return t;

        const float top    = 1.0f - std::exp2f(-k * (1.0f - t));
        const float bottom = 1.0f - std::exp2f(-k);
        return 1.0f - (top / bottom);
    }

    static float bounce(float t, float damping = 8.0f, float frequency = 3.0f) {
        t                 = clamp01(t);
        const float decay = std::exp2f(-damping * t);
        const float wave =
            std::abs(std::cos((0.5f + frequency) * constants::pi * t));
        return 1.0f - (decay * wave);
    }
};

class LerpUtil {

  public:
    // Core Linear Interpolation (No Easing)
    template <typename T, typename FloatType>
    // can the {} be evaluated and can -> return be converted back to T
        requires requires(T a, T b, FloatType t) {
            { (b - a) * t } -> std::convertible_to<T>;
            { a + (b - a) * t } -> std::convertible_to<T>;
        }
    static T lerp(T a, T b, FloatType t) {
        return a + (b - a) * t;
    }

    // Discrete
    template <typename T> static T discrete(T a, T b, float t) {
        return t < 1.0f ? a : b;
    }

    // Applied Easing any curve
    template <typename CurveFunc>
    static float ease(
        float     alpha,
        CurveFunc curve_fn = Curve::linear,
        EaseMode  mode     = EaseMode::In
    ) {
        auto t = std::clamp(alpha, 0.0f, 1.0f);

        switch (mode) {
        case EaseMode::In:
            return curve_fn(t);
        case EaseMode::Out:
            return 1.0f - curve_fn(1.0f - t);
        case EaseMode::InOut:
            if (t < 0.5f) {
                return 0.5f * curve_fn(2.0f * t);
            } else {
                return 1.0f - 0.5f * curve_fn(2.0f * (1.0f - t));
            }
        case EaseMode::OutIn:
            if (t < 0.5f) {
                return 0.5f * (1.0f - curve_fn(1.0f - 2.0f * t));
            } else {
                return 0.5f + 0.5f * curve_fn(2.0f * t - 1.0f);
            }
        default: // default to In
            return curve_fn(t);
        }
    }

    template <typename T, typename CurveFunc>
    static T eased_lerp(
        T         a,
        T         b,
        float     alpha,
        CurveFunc curve_fn,
        EaseMode  mode = EaseMode::In
    ) {
        auto t = ease(alpha, curve_fn, mode);
        return LerpUtil::lerp(a, b, t);
    }
};

} // namespace varicle

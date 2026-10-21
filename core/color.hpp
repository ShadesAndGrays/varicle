#pragma once
#include <cstdint>

namespace varicle {

// from 0 to 1
struct Colorf {

    union {
        struct {
            float r = 1.0f;
            float g = 1.0f;
            float b = 1.0f;
            float a = 1.0f;
        };
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float c[4];
    };

    float* data() { return c; }

    static constexpr Colorf White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
    static constexpr Colorf Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
    static constexpr Colorf Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
    static constexpr Colorf Orange() {
        return { 1.0f, 165.0f / 255.0f, 0.0f, 1.0f };
    }
    static constexpr Colorf Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
    static constexpr Colorf Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
    static constexpr Colorf Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
    static constexpr Colorf Purple() {
        return { 200.0f / 255.0f, 122.0f / 255.0f, 1.0f, 1.0f };
    }
};

struct Color8 {
    union {
        struct {
            uint8_t r = 255;
            uint8_t g = 255;
            uint8_t b = 255;
            uint8_t a = 255;
        };
        struct {
            uint8_t x;
            uint8_t y;
            uint8_t z;
            uint8_t w;
        };
        uint8_t c[4];
    };

    uint8_t* data() { return c; }

    // Common default presets
    static constexpr Color8 White() { return { 255, 255, 255, 255 }; }
    static constexpr Color8 Black() { return { 0, 0, 0, 255 }; }
    static constexpr Color8 Red() { return { 255, 0, 0, 255 }; }
    static constexpr Color8 Orange() { return { 255, 165, 0, 255 }; }
    static constexpr Color8 Yellow() { return { 255, 255, 0, 255 }; }
    static constexpr Color8 Green() { return { 0, 255, 0, 255 }; }
    static constexpr Color8 Blue() { return { 0, 0, 255, 255 }; }
    static constexpr Color8 Purple() { return { 200, 122, 255, 255 }; }
};

class ColorUtil {
  public:
    // Standalone Color Interpolation Helper
    static Color8 color_lerp(Color8 s, Color8 e, float a) {
        auto lerp_u8 = [](uint8_t start, uint8_t end, float t) -> uint8_t {
            return static_cast<uint8_t>(start + (end - start) * t);
        };
        return { lerp_u8(s.r, e.r, a),
                 lerp_u8(s.g, e.g, a),
                 lerp_u8(s.b, e.b, a),
                 lerp_u8(s.a, e.a, a) };
    }

    static constexpr Colorf tof(Color8 c) {
        return { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
    }

    static constexpr Color8 to8(Colorf c) {
        auto f = [](float v) {
            v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
            return static_cast<uint8_t>(v * 255.0f + 0.5f);
        };
        return { f(c.r), f(c.g), f(c.b), f(c.a) };
    }
};

} // namespace varicle

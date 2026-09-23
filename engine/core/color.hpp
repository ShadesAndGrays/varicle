#pragma once
#include <cstdint>

namespace varicle {
struct Color {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;

    // Common default presets
    static constexpr Color White() { return { 255, 255, 255, 255 }; }
    static constexpr Color Black() { return { 0, 0, 0, 255 }; }
    static constexpr Color Red() { return { 255, 0, 0, 255 }; }
    static constexpr Color Orange() { return { 255, 165, 0, 255 }; }
    static constexpr Color Yellow() { return { 255, 255, 0, 255 }; }
    static constexpr Color Green() { return { 0, 255, 0, 255 }; }
    static constexpr Color Blue() { return { 0, 0, 255, 255 }; }
    static constexpr Color Purple() { return { 200, 122, 255, 255 }; }
};

class ColorUtil {
  public:
    // Standalone Color Interpolation Helper
    static Color color_lerp(Color s, Color e, float a) {
        auto lerp_u8 = [](uint8_t start, uint8_t end, float t) -> uint8_t {
            return static_cast<uint8_t>(start + (end - start) * t);
        };
        return { lerp_u8(s.r, e.r, a),
                 lerp_u8(s.g, e.g, a),
                 lerp_u8(s.b, e.b, a),
                 lerp_u8(s.a, e.a, a) };
    }

    // static ::Color to_raylib_color(const varicle::Color& c) {
    //     return ::Color{ c.r, c.g, c.b, c.a };
    // }
};

} // namespace varicle

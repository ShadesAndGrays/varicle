#pragma once
#include "engine/core/color.hpp"
#include "engine/util/slot-map.hpp"
#include <iostream>
#include <string>
#include <variant>

#include <entt/entt.hpp>

namespace varicle {
// Overload helper pattern
template <typename... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }
    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
    Vec2 operator*(int scalar) const { return { x * scalar, y * scalar }; }

    constexpr static Vec2 ZERO() { return Vec2{ 0.0f, 0.0f }; }
    constexpr static Vec2 ONE() { return Vec2{ 1.0f, 1.0f }; }
    constexpr static Vec2 UP() { return Vec2{ 0.0f, 1.0f }; }
    constexpr static Vec2 DOWN() { return Vec2{ 0.0f, -1.0f }; }
    constexpr static Vec2 LEFT() { return Vec2{ -1.0f, 0.0f }; }
    constexpr static Vec2 RIGHT() { return Vec2{ 1.0f, 0.0f }; }
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    bool operator==(const Vec3& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
    bool operator!=(const Vec3& o) const { return !(*this == o); }
    Vec3 operator+(const Vec3& o) const {
        return { x + o.x, y + o.y, z + o.z };
    }
    Vec3 operator-(const Vec3& o) const {
        return { x - o.x, y - o.y, z - o.z };
    }
    Vec3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }
    Vec3 operator*(int scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }
};

struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    bool operator==(const Vec4& o) const {
        return x == o.x && y == o.y && z == o.z && w == o.w;
    }
    bool operator!=(const Vec4& o) const { return !(*this == o); }
    Vec4 operator+(const Vec4& o) const {
        return { x + o.x, y + o.y, z + o.z, w + o.w };
    }
    Vec4 operator-(const Vec4& o) const {
        return { x - o.x, y - o.y, z - o.z, w - o.w };
    }
    Vec4 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }
    Vec4 operator*(int scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }
};

// Standalone Interpolation Helpers
inline Vec2 vec2_lerp(const Vec2& s, const Vec2& e, float a) {
    return { s.x + (e.x - s.x) * a, s.y + (e.y - s.y) * a };
}
inline Vec3 vec3_lerp(const Vec3& s, const Vec3& e, float a) {
    return { s.x + (e.x - s.x) * a,
             s.y + (e.y - s.y) * a,
             s.z + (e.z - s.z) * a };
}
inline Vec4 vec4_lerp(const Vec4& s, const Vec4& e, float a) {
    return { s.x + (e.x - s.x) * a,
             s.y + (e.y - s.y) * a,
             s.z + (e.z - s.z) * a,
             s.w + (e.w - s.w) * a };
}

class EngineVariant {

  public:
    using InternalVariant = std::variant<
        std::monostate,
        int,
        float,
        Vec2,
        Vec3,
        Vec4,
        std::string,
        bool,
        varicle::Color,
        entt::entity,
        structures::SlotID>;

    EngineVariant() : data(std::monostate{}) {}
    EngineVariant(entt::entity v) : data(v) {}
    EngineVariant(int v) : data(v) {}
    EngineVariant(float v) : data(v) {}
    EngineVariant(Vec2 v) : data(v) {}
    EngineVariant(Vec3 v) : data(v) {}
    EngineVariant(Vec4 v) : data(v) {}
    EngineVariant(std::string v) : data(std::move(v)) {}
    EngineVariant(const char* v) : data(std::string(v)) {}
    EngineVariant(bool v) : data(v) {}
    EngineVariant(Color v) : data(v) {}
    EngineVariant(structures::SlotID v) : data(v) {}

    template <typename T> T        get() const { return std::get<T>(data); }
    template <typename T> const T* try_get() const {
        return std::get_if<T>(&data);
    }

    operator int() const { return get<int>(); }
    operator float() const { return get<float>(); }
    operator Vec2() const { return get<Vec2>(); }
    operator Vec3() const { return get<Vec3>(); }
    operator Vec4() const { return get<Vec4>(); }
    operator Color() const { return get<Color>(); }
    operator entt::entity() const { return get<entt::entity>(); }
    operator bool() const { return get<bool>(); }
    operator std::string() const { return get<std::string>(); }

    std::string to_string() const {
        return std::visit(
            Overloaded{
                [](std::monostate) { return std::string("!"); },
                [](entt::entity v) {
                    return std::to_string(static_cast<uint32_t>(v));
                },
                [](int v) { return std::to_string(v); },
                [](float v) { return std::to_string(v); },

                [](const Vec2& v) {
                    return std::to_string(v.x) + ", " + std::to_string(v.y);
                },
                [](const Vec3& v) {
                    return std::to_string(v.x) + ", " + std::to_string(v.y) +
                        ", " + std::to_string(v.z);
                },
                [](const Vec4& v) {
                    return std::to_string(v.x) + ", " + std::to_string(v.y) +
                        ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
                },
                [](const varicle::Color& v) {
                    return std::to_string(v.r) + ", " + std::to_string(v.g) +
                        ", " + std::to_string(v.b) + ", " + std::to_string(v.a);
                },
                [](const std::string& s) { return s; },
                [](const bool& b) { return std::string(b ? "true" : "false"); },
                [](const structures::SlotID& v) { return v.to_string(); } },
            data
        );
    }

    friend std::ostream&
    operator<<(std::ostream& os, const EngineVariant& variant) {
        return os << variant.get_type_name() << "(" << variant.to_string()
                  << ")";
    }

  private:
    InternalVariant data;

    std::string get_type_name() const {
        return std::visit(
            Overloaded{
                [](std::monostate) -> std::string { return "Null"; },
                [](bool) -> std::string { return "Bool"; },
                [](entt::entity) -> std::string { return "Entity"; },
                [](int) -> std::string { return "Int"; },
                [](float) -> std::string { return "Float"; },
                [](const Vec2&) -> std::string { return "Vec2"; },
                [](const Vec3&) -> std::string { return "Vec3"; },
                [](const Vec4&) -> std::string { return "Vec4"; },
                [](const varicle::Color&) -> std::string { return "Color"; },
                [](const std::string&) -> std::string { return "String"; },
                [](const structures::SlotID&) -> std::string { return "ID"; },
                [](const auto&) -> std::string { return "Unknown"; } },
            data
        );
    }

    friend class VariantOpManager;
};
} // namespace varicle

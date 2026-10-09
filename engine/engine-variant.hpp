#pragma once
#include "core/color.hpp"
#include "core/slot-map.hpp"
#include "core/type.hpp"
#include <iostream>
#include <string>
#include <variant>

// #include <entt/entt.hpp>

namespace varicle {
// Overload helper pattern
template <typename... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

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
        varicle::Color8,
        // entt::entity,
        structures::SlotID>;

    EngineVariant() : data(std::monostate{}) {}
    // EngineVariant(entt::entity v) : data(v) {}
    EngineVariant(int v) : data(v) {}
    EngineVariant(float v) : data(v) {}
    EngineVariant(Vec2 v) : data(v) {}
    EngineVariant(Vec3 v) : data(v) {}
    EngineVariant(Vec4 v) : data(v) {}
    EngineVariant(std::string v) : data(std::move(v)) {}
    EngineVariant(const char* v) : data(std::string(v)) {}
    EngineVariant(bool v) : data(v) {}
    EngineVariant(Color8 v) : data(v) {}
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
    operator Color8() const { return get<Color8>(); }
    // operator entt::entity() const { return get<entt::entity>(); }
    operator bool() const { return get<bool>(); }
    operator std::string() const { return get<std::string>(); }

    std::string to_string() const {
        return std::visit(
            Overloaded{
                [](std::monostate) { return std::string("!"); },
                // [](entt::entity v) {
                //     return std::to_string(static_cast<uint32_t>(v));
                // },
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
                [](const varicle::Color8& v) {
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
                // [](entt::entity) -> std::string { return "Entity"; },
                [](int) -> std::string { return "Int"; },
                [](float) -> std::string { return "Float"; },
                [](const Vec2&) -> std::string { return "Vec2"; },
                [](const Vec3&) -> std::string { return "Vec3"; },
                [](const Vec4&) -> std::string { return "Vec4"; },
                [](const varicle::Color8&) -> std::string { return "Color"; },
                [](const std::string&) -> std::string { return "String"; },
                [](const structures::SlotID&) -> std::string { return "ID"; },
                [](const auto&) -> std::string { return "Unknown"; } },
            data
        );
    }

    friend class VariantOpManager;
};
} // namespace varicle

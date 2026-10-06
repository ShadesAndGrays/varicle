// Generated using claude. Review later
#pragma once

#include <cstdint>
#include <string_view>
namespace varicle::core {

using hash_t = std::uint32_t;

constexpr hash_t fnv1a(const char* str, size_t len) noexcept {
    hash_t h = 2166136261u; // FNV offset basis
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<hash_t>(static_cast<unsigned char>(str[i]));
        h *= 16777619u; // FNV prime
    }
    return h;
}

constexpr hash_t hash(std::string_view s) noexcept {
    return fnv1a(s.data(), s.size());
}

namespace literals {
constexpr hash_t operator""_hs(const char* str, std::size_t len) noexcept {
    return fnv1a(str, len);
}
} // namespace literals
  //
} // namespace varicle::core

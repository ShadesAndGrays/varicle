#pragma once

#include "core/type.hpp"
#include <cstddef>
#include <functional>
#include <vector>

namespace varicle::renderer {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal &&
            uv == other.uv;
    }
};

struct Mesh {
    std::vector<Vertex>   verticies;
    std::vector<uint32_t> indices;
};

enum PrimitiveType {
    TRIANGLE,
    QUAD,
    TWO_QUAD_CUBE,
    FULLSCREEN_TRIANGLE,
    LAST,
};

Mesh get_primitive(PrimitiveType type);

} // namespace varicle::renderer

namespace std {
template <size_t N> struct ArrayHash {
    size_t operator()(const std::array<float, N>& a) {
        size_t h = 0;

        for (float x : a) {
            size_t hx = std::hash<float>{}(x);
            h ^= hx + 0x9e3779b9 + (h << 6) + (h >> 2);
        }

        return h;
    }
};

template <> struct hash<varicle::renderer::Vertex> {
    size_t operator()(varicle::renderer::Vertex const& vertex) const {
        size_t h_pos  = ArrayHash<3>{}(vertex.position.v);
        size_t h_norm = ArrayHash<3>{}(vertex.normal.v);
        size_t h_uv   = ArrayHash<2>{}(vertex.uv.v);
        return ((h_pos ^ (h_norm << 1)) >> 1) ^ (h_uv << 1);
    }
};

} // namespace std

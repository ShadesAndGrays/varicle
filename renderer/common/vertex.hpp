#pragma once

#include <vector>
namespace varicle::renderer {

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
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

#pragma once

#include "renderer/common/color.hpp"
#include "renderer/common/window.hpp"
#include "renderer/opengl/shader.hpp"
#include <cstdint>
#include <vector>

namespace varicle::renderer::opengl {

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
};

struct MeshData {
    uint32_t              ID;
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    unsigned int          vbo;
    unsigned int          vao;
    unsigned int          ebo;
};

struct TextureData {
    uint32_t ID;
    int32_t  width;
    int32_t  height;
    int32_t  channels;
};

struct GLContext {
    renderer::Window& window;
    MeshData          meshes[512];
    TextureData       Textures[512];
    Shader            Shaders[512];

    GLContext(Window& window) : window(window) {}
    renderer::Color clear_color = { 1, 1, 1, 1 };
};

GLContext init(Window& window);

bool should_close(GLContext& ctx);
void begin_frame(GLContext& ctx);
void end_frame(GLContext& ctx);
void cleanup(GLContext& ctx);
void set_clear_color(GLContext& ctx, renderer::Color color);

} // namespace varicle::renderer::opengl

#pragma once

#include "renderer/common/color.hpp"
#include "renderer/common/vertex.hpp"
#include "renderer/common/window.hpp"
#include "renderer/opengl/shader.hpp"
#include <cstdint>
#include <vector>

namespace varicle::renderer::opengl {
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
    Window&                  window;
    std::vector<MeshData>    meshes;
    std::vector<TextureData> Textures;
    std::vector<Shader>      Shaders;

    GLContext(Window& window) : window(window) {}
    Color clear_color = { 1, 1, 1, 1 };
};

GLContext init(Window& window);

bool should_close(GLContext& ctx);
void begin_frame(GLContext& ctx);
void end_frame(GLContext& ctx);
void draw_primitive(GLContext& ctx, PrimitiveType primitive);
void cleanup(GLContext& ctx);
void set_clear_color(GLContext& ctx, Color color);

} // namespace varicle::renderer::opengl

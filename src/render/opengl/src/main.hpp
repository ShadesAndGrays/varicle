#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <vector>

#include "shader.hpp"

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

struct Context {
    GLFWwindow* window;
    MeshData    meshes[512];
    TextureData Textures[512];
    Shader      Shaders[512];
};

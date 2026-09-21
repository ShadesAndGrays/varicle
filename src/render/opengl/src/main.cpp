#include "transform.hpp"
#include <expected>
#include <glad/glad.h>

#include "main.hpp"
#include "util.hpp"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <format>
#include <print>

namespace fs = std::filesystem;
using std::println;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void setup_vertex_data(MeshData& mesh_data) {
    glGenVertexArrays(1, &mesh_data.vao);
    glGenBuffers(1, &mesh_data.vbo);
    glGenBuffers(1, &mesh_data.ebo);

    glad_glBindVertexArray(mesh_data.vao);

    glad_glBindBuffer(GL_ARRAY_BUFFER, mesh_data.vbo);
    glad_glBufferData(
        GL_ARRAY_BUFFER,
        mesh_data.vertices.size() * sizeof(Vertex),
        mesh_data.vertices.data(),
        GL_STATIC_DRAW
    );

    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data.ebo);
    glad_glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh_data.indices.size() * sizeof(uint32_t),
        mesh_data.indices.data(),
        GL_STATIC_DRAW
    );

    glad_glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );
    glad_glEnableVertexAttribArray(0);

    glad_glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );
    glad_glEnableVertexAttribArray(1);

    glad_glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv)
    );
    glad_glEnableVertexAttribArray(2);
}

MeshData make_quad() {

    MeshData mesh_data{};

    /*
     * 0----1
     * |    |
     * 3----2
     */
    mesh_data.vertices = {
        { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
    };

    mesh_data.indices = { 0, 1, 2, 0, 2, 3 };

    setup_vertex_data(mesh_data);

    println(
        "Made quad vb: {} va:{} eb:{}",
        mesh_data.vbo,
        mesh_data.vao,
        mesh_data.ebo
    );
    return mesh_data;
}

void bind_texture(Context& ctx, uint32_t texture) {
    glad_glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.Textures[texture].ID);
}

std::expected<TextureData, std::string>
load_texture_data(const std::string& file_path) {
    auto image = load_image(file_path);
    if (!image.has_value()) {
        return std::unexpected<std::string>(image.error());
    }

    auto [w, h, c, data] = std::move(image.value());

    TextureData texture;

    glGenTextures(1, &texture.ID);
    glad_glBindTexture(GL_TEXTURE_2D, texture.ID);

    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glad_glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR
    );

    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glad_glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data.get()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    texture.width    = w;
    texture.height   = h;
    texture.channels = c;

    return texture;
}

void load(Context& ctx) {

    int has_glfw = glfwInit();
    if (has_glfw == GLFW_FALSE) {
        println("Couldn't load glfw");
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ctx.window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);
    glfwMakeContextCurrent(ctx.window);

    int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (version == 0) {
        println("Couldn't load OpenGL context");
        exit(-1);
    }

    println(
        "Successfully loaded Opengl {} {}", GLVersion.major, GLVersion.minor
    );

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(ctx.window, framebuffer_resize_callback);

    glad_glEnable(GL_BLEND);
    glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

template <typename T> T* addr(T&& t) {
    return &t;
}

void cleanup(Context& ctx) {

    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

std::expected<Shader, std::string>
load_shader(std::string vert, std::string frag) {

    const char* vertc = vert.c_str();
    const char* fragc = frag.c_str();

    auto vert_shader = glad_glCreateShader(GL_VERTEX_SHADER);
    auto frag_shader = glad_glCreateShader(GL_FRAGMENT_SHADER);

    int  success = 0;
    char info_log[512];

    glad_glShaderSource(vert_shader, 1, &vertc, NULL);

    glad_glCompileShader(vert_shader);
    glad_glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glad_glGetShaderInfoLog(vert_shader, sizeof(info_log), NULL, info_log);
        return std::unexpected<std::string>(
            std::format("Failed to compile vertex shader {}", info_log)
        );
    }

    glad_glShaderSource(frag_shader, 1, &fragc, NULL);

    glad_glCompileShader(frag_shader);
    glad_glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glad_glGetShaderInfoLog(frag_shader, sizeof(info_log), NULL, info_log);
        return std::unexpected<std::string>(
            std::format("Failed to compile fragment shader {}", info_log)
        );
    }

    auto program = glad_glCreateProgram();

    glad_glAttachShader(program, vert_shader);
    glad_glAttachShader(program, frag_shader);
    glad_glLinkProgram(program);

    glad_glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glad_glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        return std::unexpected<std::string>(
            std::format("Failed to link program {}", info_log)
        );
    }

    glad_glDeleteShader(vert_shader);
    glad_glDeleteShader(frag_shader);

    println("Shader created {}", program);

    return Shader{ program };
}

std::expected<Shader, std::string> load_shaders_from_file(
    std::string vertex_shader_path,
    std::string fragment_shader_path
) {

    auto vert = read_file(vertex_shader_path);

    if (!vert.has_value()) {
        return std::unexpected<std::string>("Failed to load vert shader");
    }

    auto frag = read_file(fragment_shader_path);

    if (!frag.has_value()) {
        return std::unexpected<std::string>("Failed to load vert shader");
    }
    return load_shader(vert.value(), frag.value());
}

int load_all_images(Context& ctx) {
    std::filesystem::directory_entry entry("textures");
    if (!entry.exists()) {
        println("No directory");
        return 0;
    }

    int count = 0;
    for (auto i : fs::directory_iterator("textures")) {
        auto result = load_texture_data(i.path().string());
        if (result.has_value()) {
            ctx.Textures[count] = result.value();
            count += 1;
        }
    }
    return count;
}

int main() {

    Context ctx;
    load(ctx);
    auto quad_mesh = make_quad();

    auto result =
        load_shaders_from_file("shaders/vert.glsl", "shaders/frag.glsl");
    if (!result.has_value()) {
        println("{}", result.error());
        exit(-1);
    }
    ctx.Shaders[0] = result.value();

    auto texture_result = load_texture_data("textures/bird.png");

    if (!texture_result.has_value()) {
        println("{}", texture_result.error());
        exit(-1);
    }

    auto texture_count = load_all_images(ctx);

    ctx.Textures[0] = texture_result.value();

    glad_glDisable(GL_CULL_FACE);

    float current_texture = 0;
    auto  quad_transform  = Transform{
        { 0, 0, 0 },
        { 1, 1, 1 },
        { 0, 0, 0 },
    };

    ctx.Shaders[0].use();
    while (!glfwWindowShouldClose(ctx.window)) {
        glad_glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glad_glClear(GL_COLOR_BUFFER_BIT);

        glad_glBindVertexArray(quad_mesh.vao);

        ctx.Shaders[0].use();
        current_texture += 0.01;
        quad_transform.rotation[2] = current_texture;
        
        current_texture = fmod(current_texture, texture_count);

        bind_texture(ctx, uint32_t(floor(current_texture)));

        ctx.Shaders[0].set("u_transform", get_model_matrix(quad_transform));

        glad_glDrawElements(
            GL_TRIANGLES, quad_mesh.indices.size(), GL_UNSIGNED_INT, 0
        );
        glad_glBindVertexArray(0);

        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    cleanup(ctx);
}

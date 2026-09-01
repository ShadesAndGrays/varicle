#include "model.hpp"
#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace varicle::render::vulkan {

void load_model(VulkanContext& ctx) {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      warn, err;

    if (!tinyobj::LoadObj(
            &attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()
        )) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };

            vertex.tex_coord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };

            vertex.color = {
                1.0f,
                1.0f,
                1.0f,
            };

            auto [it, inserted] = unique_vertices.insert(
                { vertex, static_cast<uint32_t>(ctx.vertices.size()) }
            );
            if (inserted) {
                ctx.vertices.push_back(vertex);
            }
            ctx.indices.push_back(it->second);
        }
    }
}

} // namespace varicle::render::vulkan

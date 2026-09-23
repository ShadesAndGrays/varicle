#pragma once
#include <cstdint>

namespace varicle::renderer {
using TextureHandle  = uint32_t;
using MeshHandle     = uint32_t;
using MaterialHandle = uint32_t;
using ShaderHandle   = uint32_t;

constexpr TextureHandle  INVALID_TEXTURE  = 0;
constexpr MeshHandle     INVALID_MESH     = 0;
constexpr MaterialHandle INVALID_MATERIAL = 0;
constexpr ShaderHandle   INVALID_SHADER   = 0;
} // namespace varicle::render

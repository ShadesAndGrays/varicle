#pragma once
#include "core/slot-map.hpp"

namespace varicle::renderer {
using TextureHandle   = structures::SlotID;
using MeshHandle      = structures::SlotID;
using MaterialHandle  = structures::SlotID;
using ShaderHandle    = structures::SlotID;
using RenderCamHandle = structures::SlotID;

constexpr TextureHandle   INVALID_TEXTURE       = structures::INVALID_SLOTID;
constexpr MeshHandle      INVALID_MESH          = structures::INVALID_SLOTID;
constexpr MaterialHandle  INVALID_MATERIAL      = structures::INVALID_SLOTID;
constexpr ShaderHandle    INVALID_SHADER        = structures::INVALID_SLOTID;
constexpr RenderCamHandle INVALID_RENDER_CAMERA = structures::INVALID_SLOTID;
} // namespace varicle::renderer

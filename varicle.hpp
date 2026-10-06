#pragma once
// IWYU pragma: begin_exports

#ifdef VARICLE_MODULE_ENGINE
#include "engine/application.hpp"
#include "engine/asset/asset.hpp"
#include "engine/asset/raylib-asset.hpp"
#include "engine/ecs/components.hpp"
#include "engine/physics/physics-server.hpp"
#include "engine/render/render-system.hpp"
#include "engine/scene/scene.hpp"
#include "engine/ui/ui.hpp"
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <imgui.h>
#include <json.hpp>
#endif

#ifdef VARICLE_MODULE_CORE
#include "core/core.hpp"
#endif

#include "renderer/render.hpp"

// IWYU pragma: end_exports

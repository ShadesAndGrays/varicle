
// IWYU pragma: begin_exports

#include "engine/asset/asset.hpp"
#include "engine/asset/raylib-asset.hpp"

#include "engine/core/application.hpp"
#include "engine/core/color.hpp"
#include "engine/core/constants.hpp"
#include "engine/core/event-bus.hpp"
#include "engine/core/lerp.hpp"
#include "engine/core/service-locator.hpp"

#include "engine/core/engine-variant/engine-variant-operation.hpp"
#include "engine/core/engine-variant/engine-variant-property-id.hpp"
#include "engine/core/engine-variant/engine-variant-property.hpp"
#include "engine/core/engine-variant/engine-variant.hpp"

#include "engine/ecs/components.hpp"
#include "engine/physics/physics-server.hpp"
#include "engine/render/render-system.hpp"
#include "engine/scene/scene.hpp"

#include "engine/util/math_util.hpp"
#include "engine/util/slot-map.hpp"
#include "engine/util/sprite_util.hpp"

#include "engine/ui/ui.hpp"

#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <imgui.h>
#include <json.hpp>

#include "renderer/render.hpp"

// IWYU pragma: end_exports

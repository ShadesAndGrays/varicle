#pragma once

#include "engine/core/engine-variant/engine-variant-property.hpp"

namespace varicle {
// Single point of initialization for all engine component properties
void register_all_component_properties(PropertyDatabase& db);
void register_all_ui_properties(PropertyDatabase& db);
} // namespace varicle

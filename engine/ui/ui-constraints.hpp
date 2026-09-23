#pragma once

#include <limits>

namespace varicle::ui {

struct LayoutConstraint {
    float min_width  = 0.0f;
    float max_width  = std::numeric_limits<float>::infinity();
    float min_height = 0.0f;
    float max_height = std::numeric_limits<float>::infinity();
};
} // namespace varicle::ui

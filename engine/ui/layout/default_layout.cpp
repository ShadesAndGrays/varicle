#include "default_layout.hpp"
#include "engine/ui/node.hpp"
#include <algorithm>

using namespace varicle;
using namespace varicle::ui;

Vec2 DefaultLayout::measure(
    std::vector<std::shared_ptr<UINode>>& nodes,
    LayoutConstraint                      constraint
) {
    Vec2 preferred_size = Vec2::ZERO();

    for (auto& node : nodes) {
        node->measure_layout(constraint);
        Vec2 node_prefered_size = node->preferred_size;
        preferred_size.x = std::max(preferred_size.x, node_prefered_size.x);
        preferred_size.y = std::max(preferred_size.y, node_prefered_size.y);
    }

    return { preferred_size.x, preferred_size.y };
}

void DefaultLayout::arrange(
    std::vector<std::shared_ptr<UINode>>& nodes,
    Rect                                  rect
) {
    Rect child_rect = rect;

    for (auto& node : nodes) {

        child_rect.width = std::clamp(node->preferred_size.x, 0.0f, rect.width),

        child_rect.height =
            std::clamp(node->preferred_size.y, 0.0f, rect.height),
        node->update_layout(child_rect);
    }
}

#include "fill_layout.hpp"
#include "default_layout.hpp"
#include "engine/ui/node.hpp"

using namespace varicle;
using namespace varicle::ui;

Vec2 varicle::ui::FillStrategy::measure(
    std::vector<std::shared_ptr<UINode>>& nodes,
    LayoutConstraint                      constraint
) {
    return DefaultLayout().measure(nodes, constraint);
}

void FillStrategy::arrange(
    std::vector<std::shared_ptr<UINode>>& nodes,
    Rect                                  rect
) {
    for (auto& node : nodes) {
        Rect child_rect = rect;
        switch (node->container_sizing.horizontal) {
        case EXACT:
            child_rect.width = node->preferred_size.x;
            break;
        case FILL:
            break;
        }

        switch (node->container_sizing.vertical) {
        case EXACT:
            child_rect.height = node->preferred_size.y;
        case FILL:
            break;
        }

        node->update_layout(child_rect);
    }
}

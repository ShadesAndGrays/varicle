#include "hstacking_layout.hpp"
#include "default_layout.hpp"
#include "engine/ui/node.hpp"
#include "engine/ui/strucutres.hpp"

using namespace varicle;
using namespace varicle::ui;

Vec2 HStackingStrategy::measure(
    std::vector<std::shared_ptr<UINode>>& nodes,
    LayoutConstraint                      constraint
) {
    // return DefaultLayout().measure(nodes, constraint);

    Vec2 preferred_size = Vec2::ZERO();

    for (auto& node : nodes) {
        node->measure_layout(constraint);
        Vec2 node_prefered_size = node->preferred_size;
        preferred_size.x += node_prefered_size.x;
        preferred_size.y = std::max(preferred_size.y, node_prefered_size.y);
    }

    return { preferred_size.x, preferred_size.y };
}
void HStackingStrategy::arrange(
    std::vector<std::shared_ptr<UINode>>& nodes,
    Rect                                  rect
) {
    const int number_of_children = nodes.size();

    if (number_of_children == 0)
        return;

    Rect child_rect = {
        rect.x,
        rect.y,
        rect.width,
        rect.height,
    };

    // free space ratio total
    float slack_ratio = 0.0f;
    // the space the children take
    float actual_child_space = 0.0f;

    // acutal free space
    float slack_space = rect.width;

    for (auto& node : nodes) {
        if (node->container_sizing.horizontal == ui::SizingMode::FILL) {
            slack_ratio += node->bias;
        } else {
            slack_space -= node->preferred_size.x;
        }
        actual_child_space += node->preferred_size.x;
    }

    auto parent = nodes[0]->get_parent().lock();

    if (parent) {
        switch (parent->alignment) {
        case START:
            break;
        case CENTER:
            child_rect.x += (rect.width - actual_child_space) / 2.0f;
            break;
        case END:
            child_rect.x += (rect.width - actual_child_space);
            break;
        }
    }
    if (slack_ratio <= 0.0f) {
        slack_ratio = 1;
    }

    for (auto i = 0; i < nodes.size(); i++) {

        switch (nodes[i]->container_sizing.horizontal) {

        case EXACT:
            child_rect.width = nodes[i]->preferred_size.x;
            break;

        case FILL:
            child_rect.width = std::max(
                (slack_space * (nodes[i]->bias / slack_ratio)),
                nodes[i]->preferred_size.x
            );
            break;
        }

        switch (nodes[i]->container_sizing.vertical) {
        case EXACT:
            child_rect.height = nodes[i]->preferred_size.y;
            break;

        case FILL:
            child_rect.height = rect.height;
            break;
        }

        nodes[i]->update_layout(child_rect);
        child_rect.x += child_rect.width;
    }
}

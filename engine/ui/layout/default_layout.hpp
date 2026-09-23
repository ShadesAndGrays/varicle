#pragma once

#include "base_layout.hpp"

namespace varicle::ui {

class DefaultLayout : public ILayoutStrategy {

  public:
    Vec2 measure(
        std::vector<std::shared_ptr<UINode>>& nodes,
        LayoutConstraint                      constraint
    ) override;

    void
    arrange(std::vector<std::shared_ptr<UINode>>& nodes, Rect rect) override;
};
} // namespace varicle::ui

#pragma once

#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/ui/ui-constraints.hpp"
#include "engine/ui/strucutres.hpp"
#include <memory>
#include <vector>

namespace varicle::ui {

class UINode;

class ILayoutStrategy {
  private:
  public:
    virtual ~ILayoutStrategy() = default;

    virtual Vec2 measure(
        std::vector<std::shared_ptr<UINode>>& nodes,
        LayoutConstraint                      constraint
    ) = 0;

    virtual void
    arrange(std::vector<std::shared_ptr<UINode>>& nodes, Rect rect) = 0;
};

} // namespace varicle::ui


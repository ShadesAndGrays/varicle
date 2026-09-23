
#pragma once
// #include "engine/ui/node.hpp"
#include "engine/ui/panel.hpp"

namespace varicle::ui {

class UIDirectionalBox : public UIPanel {

  protected:
    virtual constexpr bool is_vertical() const = 0;
    float                  gap                 = 10.0f;

  public:
    virtual void update_layout(Rect parent_rect) {

        float anchor_left = parent_rect.x + (parent_rect.width * anchor.min_x);
        float anchor_top  = parent_rect.y + (parent_rect.height * anchor.min_y);
        float anchor_right = parent_rect.x + (parent_rect.width * anchor.max_x);
        float anchor_bottom =
            parent_rect.y + (parent_rect.height * anchor.max_y);

        m_global_rect.x     = anchor_left + offset_left;
        m_global_rect.width = (anchor_right - offset_right) - m_global_rect.x;
        m_global_rect.y     = anchor_top + offset_top;
        m_global_rect.height =
            (anchor_bottom - offset_bottom) - m_global_rect.y;

        const auto child_count = static_cast<int>(m_children.size());
        if (child_count <= 0) {
            return;
        }
        const auto child_rect_height = m_global_rect.height / child_count;
        const auto child_rect_width  = m_global_rect.width / child_count;

        for (auto i = 0; i < m_children.size(); i++) {

            auto& child      = m_children[i];
            auto  child_rect = m_global_rect;

            if (is_vertical()) {
                child_rect.height = child_rect_height;
                child_rect.y += child_rect_height * i;
            } else {
                child_rect.width = child_rect_width;
                child_rect.x += child_rect_width * i;
            }
            if (child->visible) {
#ifdef OLD
                child->update_layout(child_rect);
#endif
            }
        }
    }
};

class UIVBox : public UIDirectionalBox {
  protected:
    constexpr bool is_vertical() const override { return true; }
};

class UIHBox : public UIDirectionalBox {
  protected:
    constexpr bool is_vertical() const override { return false; }
};

} // namespace varicle::ui

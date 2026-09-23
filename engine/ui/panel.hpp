#pragma once
#include "engine/ui/node.hpp"

namespace varicle::ui {

class UIPanel : public UINode {
  public:
    varicle::Color bg_color        = { 40, 40, 40, 200 };
    bool           draw_background = true;

  protected:
    void draw_self() const override {
        if (draw_background) {
            // DrawRectangleRec(
            //     Rectangle{
            //         m_global_rect.x,
            //         m_global_rect.y,
            //         m_global_rect.width,
            //         m_global_rect.height,
            //     },
            //     ColorUtil::to_raylib_color(bg_color)
            // );
        }
    }
};

} // namespace varicle::ui

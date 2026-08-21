#pragma once
#include "engine/ui/layout/hstacking_layout.hpp"
#include "engine/ui/layout/vstacking_layout.hpp"
#include "engine/ui/node.hpp"
#include "engine/ui/panel.hpp"
#include "engine/ui/text.hpp"
#include <functional>

namespace varicle::ui::element {

class UIButton : public UINode {
  public:
    std::function<void()> on_click;
    varicle::Color        idle_bg_color  = { 150, 150, 150, 255 };
    varicle::Color        idle_fg_color  = { 255, 255, 255, 255 };
    varicle::Color        hover_bg_color = { 200, 200, 230, 255 };
    varicle::Color        hover_fg_color = { 255, 255, 255, 255 };
    varicle::Color        click_bg_color = { 200, 200, 255, 255 };
    varicle::Color        click_fg_color = { 255, 255, 255, 255 };

  private:
    bool                     m_hovered = false;
    bool                     m_pressed = false;
    std::shared_ptr<UIText>  m_text;

  public:
    UIButton(std::string text) {
        m_text          = std::make_shared<UIText>(text);
        layout_strategy = std::make_unique<VStackingStrategy>();
        alignment = CENTER;
    }

    void init() {
        // m_panel_center->self_visible = false;
        add_child(m_text);
    }
    std::shared_ptr<UIButton> static create(std::string text = "") {
        auto button = std::make_shared<UIButton>(text);
        button->init();
        return button;
    }

    bool process_input(Vec2 mouse_pos) override {
        if (UINode::process_input(mouse_pos))
            return true;

        m_hovered = CheckCollisionPointRec(
            { mouse_pos.x, mouse_pos.y },
            { m_global_rect.x,
              m_global_rect.y,
              m_global_rect.width,
              m_global_rect.height }
        );

        if (m_hovered) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_pressed = true;
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && m_pressed) {
                m_pressed = false;
                if (on_click)
                    on_click(); // Fire event!
            }
            return true; // Consume input focus so clicks don't hit physics
                         // world below
        }

        m_pressed = false;
        return false;
    }

  protected:
    void draw_self() const override {
        varicle::Color bg_col = {};
        varicle::Color fg_col = {};

        if (m_hovered) {
            if (m_pressed) {
                bg_col = click_bg_color;
                fg_col = click_fg_color;

            } else {
                bg_col = hover_bg_color;
                fg_col = hover_fg_color;
            }

        } else {
            bg_col = idle_bg_color;
            fg_col = idle_fg_color;
        }

        auto rec = Rectangle{ m_global_rect.x,
                              m_global_rect.y,
                              m_global_rect.width,
                              m_global_rect.height };
        DrawRectangleRec(rec, ColorUtil::to_raylib_color(bg_col));
        DrawRectangleLinesEx(rec, 1.0f, ColorUtil::to_raylib_color(fg_col));

        // Draw centered text
        // int font_size  = 18;
        // int text_width = MeasureText(text.c_str(), font_size);
        // DrawText(
        //     text.c_str(),
        //     static_cast<int>(
        //         m_global_rect.x + (m_global_rect.width - text_width) / 2.0f
        //     ),
        //     static_cast<int>(
        //         m_global_rect.y + (m_global_rect.height - font_size) / 2.0f
        //     ),
        //     font_size,
        //     ::BLACK
        // );
    }
};

} // namespace varicle::ui::element

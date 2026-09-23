#pragma once
#include "node.hpp"
// #include "raylib.h"

namespace varicle::ui {

class UIText : public UINode {

  private:
    std::string    m_text;
    varicle::Color m_text_color;
    int            m_font_size;

  public:
    UIText() : m_text("") {}
    UIText(std::string text) : UINode(), m_text(std::move(text)) {
        container_sizing = { EXACT, EXACT };
    }

    Vec2 get_content_size() const override {
        return {0,0};
        // auto default_font = GetFontDefault();
        // auto size         = MeasureTextEx(default_font, m_text.c_str(), 18, 1);
        // return { size.x, size.y };
    }

  protected:
    void draw_self() const override {

        // DrawTextCodepoint(Font font, int codepoint, Vector2 position, float
        // fontSize, Color tint) DrawTextEx(Font font, const char *text, Vector2
        // position, float fontSize, float spacing, Color tint)

        // DrawRectangleRec(
        //     Rectangle{ m_global_rect.x,
        //                m_global_rect.y,
        //                m_global_rect.width,
        //                m_global_rect.height },
        //     ::PURPLE
        // );
        // DrawText(
        //     m_text.c_str(),
        //     static_cast<int>(m_global_rect.x),
        //     static_cast<int>(m_global_rect.y),
        //     m_font_size,
        //     ::BLACK
        // );
    }
};

} // namespace varicle::ui

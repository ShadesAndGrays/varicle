#pragma once
#include <array>
#include <format>
#include <string>
namespace varicle::ui {

struct UIAnchor {
    // 0.0 = Left/Top, 0.5 = Center, 1.0 = Right/Bottom
    float min_x = 0.0f, min_y = 0.0f; // Parent anchor start
    float max_x = 0.0f, max_y = 0.0f; // Parent anchor end
};

// Common Anchor Presets
namespace anchors {
inline const UIAnchor TopLeft{ 0.0f, 0.0f, 0.0f, 0.0f };
inline const UIAnchor TopRight{ 1.0f, 0.0f, 1.0f, 0.0f };
inline const UIAnchor BottomLeft{ 0.0f, 1.0f, 0.0f, 1.0f };
inline const UIAnchor BottomRight{ 1.0f, 1.0f, 1.0f, 1.0f };
inline const UIAnchor Center{ 0.5f, 0.5f, 0.5f, 0.5f };
inline const UIAnchor FullStretch{ 0.0f, 0.0f, 1.0f, 1.0f };
} // namespace anchors

// How node should arrange child node
enum Aligment { START, CENTER, END };
// How node size reacts
enum SizingMode { EXACT, FILL };

struct ContainerSizing {
    SizingMode horizontal;
    SizingMode vertical;
};

using Matrix2 = std::array<std::array<float, 3>, 3>;
/*[
  [ scale_x  0        tx]
  [ 0        scale_y  ty]
  [ 0        0        1]
  ]
*/

struct Rect {
    float x, y, width, height;
    float scale_x  = 1.0f;
    float scale_y  = 1.0f;
    float rotation = 0.0f;

    // Matrix2 get_transform_matrix() {
    //
    // }

    Rect operator+(const Rect& other) {
        return Rect{
            x + other.x,
            y + other.y,
            width + other.height,
            height + other.height,
        };
    }

    Rect relative_to(const Rect& other) {
        return Rect{
            x + other.x,
            y + other.y,
            width,
            height,
        };
    }

    std::string to_string() {
        return std::format("({} {} {} {})", x, y, width, height);
    }
};
} // namespace varicle::ui

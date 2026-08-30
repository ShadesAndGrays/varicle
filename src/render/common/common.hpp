#pragma once
#include <cstdint>

namespace varicle::render {
struct Color {

    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

struct Rect {
    float x, y, width, height;
};

class IRender {
  public:
    virtual ~IRender() = default;

    virtual void
    init(uint32_t width, uint32_t height, const char* window_name) = 0;
    virtual void shutdown()                                        = 0;

    virtual void begin_frame()                 = 0;
    virtual void clear_color(Color background) = 0;

    virtual void end_frame()           = 0;
    virtual bool should_close_window() = 0;

    virtual void draw_circle(float x, float y, float radius, Color color) = 0;
    virtual void
    draw_rect(float x, float y, float widht, float height, Color color) = 0;
};

} // namespace varicle::render

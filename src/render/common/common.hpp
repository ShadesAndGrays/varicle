#pragma once
#include <cstdint>

namespace varicle::render {
struct Color {

    uint8_t r, g, b, a;
};

struct Rect {
    float x, y, width, height;
};

class IRender {
  public:
    virtual ~IRender() = default;

    virtual void
    init(void* native_window_handle, uint32_t width, uint32_t height) = 0;
    virtual void shutdown()                                           = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    virtual void draw_circle(float x, float y, float radius, Color color) = 0;
    virtual void
    draw_rect(float x, float y, float widht, float height, Color color) = 0;
};

} // namespace varicle::render

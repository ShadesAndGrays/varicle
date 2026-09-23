#include <varicle.hpp>

namespace gl = varicle::renderer::opengl;

int main() {
    varicle::renderer::Window window(800,600,"OpenGl Renderer");
    auto ctx = gl::init(window);
    gl::set_clear_color(ctx, { 0.7, 0.6, 0.9, 1.0 });
    // gl::set_clear_color(ctx, { 0.0, 0.0, 0.0, 1.0 });

    while (!gl::should_close(ctx)) {
        gl::begin_frame(ctx);
        gl::draw_primitive(ctx, varicle::renderer::PrimitiveType::TRIANGLE);
        gl::end_frame(ctx);
    }

    gl::cleanup(ctx);

    return 0;
}

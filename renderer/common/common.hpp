#pragma once

#include <glm/glm.hpp>

namespace varicle::renderer {

// class IRender {
//   public:
//     virtual void    set_camera(Camera camera) = 0;
//     virtual Camera& get_camera()              = 0;
//     virtual ~IRender()                        = default;
//
//     // Lifecycle
//     // virtual void init(Window& window)                    = 0;
//     virtual void shutdown()                              = 0;
//     virtual void resize(uint32_t width, uint32_t height) = 0;
//
//     // Frame Management
//     virtual void set_clear_color(Color background)     = 0;
//     virtual void begin_frame(bool clear_screen = true) = 0;
//     virtual void end_frame()                           = 0;
//
//     // Texture Managment
//     virtual TextureHandle load_texture(const char* filepath)     = 0;
//     virtual void          destroy_texture(TextureHandle texture) = 0;
//
//     // Mesh Managment
//     virtual MeshHandle load_mesh(const char* filepath) = 0;
//     virtual void       destroy_mesh(MeshHandle mesh)   = 0;
//
//     // Drawing Command
//     virtual void draw_object(Object object)                      = 0;
//     virtual void draw_mesh(MeshHandle mesh)                      = 0;
//     virtual void draw_rect(const Rect& rect, const Color& color) = 0;
//
//     virtual void draw_texured_rect(
//         const Rect&   rect,
//         TextureHandle texture,
//         const Color&  tint = { 1, 1, 1, 1 }
//     ) = 0;
// };

} // namespace varicle::renderer

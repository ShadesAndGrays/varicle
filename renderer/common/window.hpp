#pragma once
#include <GLFW/glfw3.h>
#include <tuple>

namespace varicle::renderer {

class Window {
  private:
    int        m_initial_width;
    int        m_initial_height;
    const char* m_initial_title;

    GLFWwindow* m_window;

  public:
    Window(int width, int height, const char* title);

    // Window(const Window&)            = delete;
    // Window& operator=(const Window&) = delete;

    Window(Window&&)            = default;
    Window& operator=(Window&&) = default;

    GLFWwindow* get_window();

    std::tuple<int, int> size();

    bool should_close_window();

    float get_aspect();

    ~Window() {
        // glfwDestroyWindow(m_window);
        // glfwTerminate();
    }
};

} // namespace varicle::renderer

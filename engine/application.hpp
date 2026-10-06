#pragma once

#include "renderer/common/window.hpp"
#include <string>

namespace varicle {

class Application {

  private:
    // Window configuration variables that a derived game can tweak in its
    // constructor

  private:
    renderer::Window m_window;

  public:
    Application(int width, int height, const char* title)
        : m_window(width, height, title) {};

    virtual ~Application() = default;

    virtual void on_init()     = 0;
    virtual void on_shutdown() = 0;

    void run();
    void change_scene(std::string scene_id);
    void quit();
};
} // namespace varicle

// src/engine/core/application.cpp

#include "engine/core/application.hpp"
#include "engine/asset/raylib-asset.hpp"
#include "engine/core/engine-variant/engine-variant-property.hpp"
#include "engine/core/event-bus.hpp"
#include "engine/core/service-locator.hpp"

#include "engine/ecs/animation.hpp"
#include "engine/ecs/bindings.hpp"
#include "engine/physics/physics-server.hpp"
#include "engine/scene/scene.hpp"
// #include "imgui.h"
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace varicle {

void game_loop(void) {

    auto& scene_manager = ServiceLocator::get<SceneManager>();
    auto& physics_sever = ServiceLocator::get<physics::PhysicsServer2D>();
    auto&  event_bus = ServiceLocator::get<event::EventBus>();

    // float dt = GetFrameTime();
    float dt = 0.01;

    // Update
    physics_sever.step(dt);
    scene_manager.update(dt);
    event_bus.flush();

    // Draw
    // BeginDrawing();
    // ClearBackground(::RAYWHITE); // Or a configurable engine default color
    //
    // scene_manager.render();
    //
    // if (physics_sever.debug)
    //     physics_sever.debug_draw_colliders();
    //
    // rlImGuiBegin();
    // scene_manager.ui();
    // rlImGuiEnd();
    //
    // EndDrawing();

    scene_manager.process_scene_switch();
}

void Application::run() {
    // 1. Initialize Raylib
    // SetTraceLogLevel(LOG_NONE);
    // SetConfigFlags(FLAG_VSYNC_HINT);
    // InitWindow(m_window_width, m_window_height, m_window_title);
    // SetTargetFPS(60);
    // rlImGuiSetup(true); // initialize gui

    // 2. Setup Service Locators
    ServiceLocator::provide(std::make_unique<PropertyDatabase>());
    ServiceLocator::provide(std::make_unique<RaylibAssetLoader>());
    ServiceLocator::provide(std::make_unique<SceneManager>());
    ServiceLocator::provide(std::make_unique<AnimationManager>());
    ServiceLocator::provide(std::make_unique<event::EventBus>());
    ServiceLocator::provide(std::make_unique<physics::PhysicsServer2D>());

    register_all_component_properties(ServiceLocator::get<PropertyDatabase>());

    // 3. Call game-specific startup (where the game creates its first scene)
    on_init();

    auto& scene_manager = ServiceLocator::get<SceneManager>();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(game_loop, 0, 1);
#else
    while (!scene_manager.should_game_close()) {
        game_loop();
    }
#endif

    // 4. Call game-specific cleanup
    on_shutdown();

    ServiceLocator::shutdown(); // Clean up services

    // 5. Close Raylib
    // rlImGuiShutdown();
    // CloseWindow();
}

void Application::change_scene(std::string scene_id) {
    ServiceLocator::get<SceneManager>().switch_to_scene(scene_id);
}

void Application::quit() {
    ServiceLocator::get<SceneManager>().quit();
}

} // namespace varicle

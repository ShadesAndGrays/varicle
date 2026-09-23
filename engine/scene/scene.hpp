#pragma once

#include <functional>

#include <entt/entt.hpp>
#include <memory>

namespace varicle {

// enum class SceneType{
//     MENU,
//     GAMEPLAY
// };
//

class Scene {

  public:
    Scene()          = default;
    virtual ~Scene() = default;

    virtual void init()           = 0;
    virtual void update(float dt) = 0;
    virtual void render()         = 0;
    virtual void ui()             = 0;
    virtual void deinit()         = 0;
};

// class GamePlayScene : public Scene{
//
//     private:
//         entt::registry registry;
//
//     public:
//         GamePlayScene();
//         ~GamePlayScene();
//
//         void update(float dt) override;
//         void render() override;
//         void ui() override;
//
// };
//
// class MenuScene : public Scene{
//
//     public:
//         MenuScene();
//         ~MenuScene() = default;
//
//         void update(float dt) override;
//         void render() override;
//         void ui() override;
//
// };

class SceneManager {
  private:
    using SceneFactory = std::function<std::unique_ptr<Scene>()>;
    std::unordered_map<std::string, SceneFactory> scene_registry;

    std::unique_ptr<Scene> current_scene = nullptr;
    std::string            next_scene_id;
    bool                   pending_switch = false;
    bool                   should_quit    = false;

  public:
    SceneManager();
    ~SceneManager();

    void   update(float dt);
    void   render();
    void   ui();
    void   process_scene_switch();
    bool   should_game_close();
    void   switch_to_scene(std::string scene_id);
    void   register_scene(std::string scene_id, SceneFactory factory);
    void   quit();
    Scene& get_current_scene();
};
} // namespace varicle

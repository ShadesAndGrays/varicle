#include "engine/scene/scene.hpp"
#include <iostream>

namespace varicle {

SceneManager::SceneManager() {
    // current_scene = std::make_unique<MenuScene>();
}

SceneManager::~SceneManager() {
    // delete current_scene;
}

Scene& SceneManager::get_current_scene() {
    return *current_scene;
}

void SceneManager::process_scene_switch() {

    if (next_scene_id.empty())
        return;

    auto it = scene_registry.find(next_scene_id);
    if (it != scene_registry.end()) {

        if (current_scene)
            current_scene->deinit();
        current_scene.reset();
        current_scene = it->second();
        current_scene->init();
    } else {
        std::cerr << std::format(
            "Scene \"{}\" does not exist\n", next_scene_id
        );
    }

    next_scene_id.clear();
}

void SceneManager::update(float dt) {
    if (!current_scene)
        return;
    current_scene->update(dt);
}

void SceneManager::render() {
    if (!current_scene)
        return;
    current_scene->render();
}

void SceneManager::ui() {
    if (!current_scene)
        return;
    current_scene->ui();
}

bool SceneManager::should_game_close() {
    // return should_quit || WindowShouldClose();
    return should_quit;
}

void SceneManager::switch_to_scene(std::string scene_id) {
    next_scene_id = scene_id;
}

void SceneManager::register_scene(std::string scene_id, SceneFactory factory) {
    scene_registry[std::string(scene_id)] = factory;
}

void SceneManager::quit() {
    should_quit = true;
}
} // namespace varicle

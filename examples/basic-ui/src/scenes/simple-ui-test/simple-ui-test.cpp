#include "simple-ui-test.hpp"

#include "varicle.hpp"

using namespace varicle;

void SimpleUITestScene::init() {
    // create root node
    root = std::make_shared<ui::UINode>();

    // set constraints
    root->layout_constraint.min_width  = 0;
    root->layout_constraint.min_width  = 0;
    root->layout_constraint.max_width  = GetScreenWidth();
    root->layout_constraint.max_height = GetScreenHeight();
    root->set_global_rect(
        { 0,
          0,
          static_cast<float>(GetScreenWidth()),
          static_cast<float>(GetScreenHeight()) }
    );

    root->set_layout_strategy(std::make_unique<ui::FillStrategy>());

    auto parent       = std::make_shared<ui::UIPanel>();
    parent->alignment = ui::Aligment::START;
    parent->set_layout_strategy(std::make_unique<ui::HStackingStrategy>());
    parent->bg_color    = varicle::Color::Black();
    parent->alignment = ui::Aligment::CENTER;
    parent->container_sizing = {ui::SizingMode::FILL, ui::SizingMode::EXACT};
    parent->bias        = 1.0f;

    // auto button = std::make_shared<ui::element::UIButton>("Play");
    auto button = ui::element::UIButton::create("play");
    button->on_click = []() {
        auto &sm = ServiceLocator::get<SceneManager>();
        sm.switch_to_scene("layout-ui");
    };

    parent->add_child(std::move(button));
    root->add_child(std::move(parent));

    // This would normally run in a loop of via and event callback;
    // Measure Pass
    root->measure_layout(root->layout_constraint);

    // Arrange Pass
    root->update_layout(root->get_global_rect());

}

void SimpleUITestScene::update(float dt) {
    root->process_input({  (float) GetMouseX(),(float)GetMouseY()});
}

void SimpleUITestScene::render() {

    // Render Pass
    root->draw();
    DrawCircleV({ 1280 / 2, 720 / 2 }, 10, ::RED);
}

void SimpleUITestScene::ui() {}

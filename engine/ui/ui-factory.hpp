#pragma once
// #include "engine/ui/ui.hpp"
#include "strucutres.hpp"
#include "panel.hpp"
#include "elements/button.hpp"
#include "node.hpp"

#include <fstream>
#include <functional>
#include <json.hpp>
#include <memory>
#include <unordered_map>

using json = nlohmann::json;

namespace varicle::ui {

inline UIAnchor string_to_anchor(const std::string& str) {
    if (str == "TopRight")
        return anchors::TopRight;
    if (str == "BottomLeft")
        return anchors::BottomLeft;
    if (str == "BottomRight")
        return anchors::BottomRight;
    if (str == "Center")
        return anchors::Center;
    if (str == "FullStretch")
        return anchors::FullStretch;
    return anchors::TopLeft; // Default
}

class UIFactory {
  public:
    using NodeCreator   = std::function<std::shared_ptr<UINode>(const json&)>;
    using EventCallback = std::function<void()>;

  private:
    std::unordered_map<std::string, NodeCreator>   m_creators;
    std::unordered_map<std::string, EventCallback> m_event_registry;

  public:
    UIFactory() {
        register_type("UINode", [](const json& j) {
            return std::make_shared<UINode>();
        });

        register_type("UIPanel", [this](const json& j) {
            auto panel = std::make_shared<UIPanel>();
            if (j.contains("bg_color")) {
                auto col        = j["bg_color"];
                panel->bg_color = Color{ col[0], col[1], col[2], col[3] };
            }
            return panel;
        });

        register_type("UIButton", [this](const json& j) {
            std::string text = j.value("text", "Button");
            auto        btn  = std::make_shared<element::UIButton>(text);
            if (j.contains("event_id")) {
                std::string event_id = j["event_id"];
                btn->on_click        = m_event_registry[event_id];
            }
            return btn;
        });
    }

    void register_type(const std::string& type_name, NodeCreator creator) {
        m_creators[type_name] = creator;
    }

    void register_event(const std::string& event_id, EventCallback callback) {
        m_event_registry[event_id] = callback;
    }

    std::shared_ptr<UINode> create_from_json(const json& j) {
        std::string type = j.value("type", "UINode");

        if (m_creators.find(type) == m_creators.end()) {
            std::cerr << "UIFactory: Unknown UI type '" << type << "'\n";
            return nullptr;
        }

        auto node     = m_creators[type](j);
        node->name    = j.value("name", "Node");
        node->visible = j.value("visible", true);

        if (j.contains("anchor")) {
            node->anchor = string_to_anchor(j["anchor"]);
        }
        if (j.contains("pivot")) {
            node->pivot.x = j["pivot"][0];
            node->pivot.y = j["pivot"][1];
        }
        if (j.contains("offset")) {
            node->offset_left   = j["offset"][0];
            node->offset_top    = j["offset"][1];
            node->offset_right  = j["offset"][2];
            node->offset_bottom = j["offset"][3];
        }

        if (j.contains("children") && j["children"].is_array()) {
            for (const auto& child_json : j["children"]) {

                auto child = create_from_json(child_json);
                if (child) {
                    node->add_child(child);
                }
            }
        }

        return node;
    }

    std::shared_ptr<UINode> load_file(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open UI file: " << filepath << "\n";
            return nullptr;
        }

        json j;
        file >> j;
        return create_from_json(j);
    }
};
} // namespace varicle::ui

#pragma once
#include "engine/core/engine-variant/engine-variant.hpp"
#include "engine/ui/layout/base_layout.hpp"
#include "engine/ui/layout/default_layout.hpp"
#include "engine/ui/strucutres.hpp"
#include "engine/ui/ui-constraints.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace varicle::ui {

class UINode : public std::enable_shared_from_this<UINode> {
  public:
    std::string name;
    bool        visible      = true;
    bool        self_visible = true;
    bool        active       = true;

    // Layout
    UIAnchor        anchor           = anchors::TopLeft;
    Aligment        alignment        = Aligment::START;
    ContainerSizing container_sizing = { SizingMode::EXACT, SizingMode::EXACT };

    Vec2 pivot = { 0.0f, 0.0f };

    std::unique_ptr<ILayoutStrategy> layout_strategy;
    LayoutConstraint                 layout_constraint;

    float offset_left   = 0.0f;
    float offset_top    = 0.0f;
    float offset_right  = 0.0f; // Width if min_x == max_x
    float offset_bottom = 0.0f; // Height if min_y == max_y

    //
    float bias = 0.0f;

    float margin_left   = 0.0f;
    float margin_top    = 0.0f;
    float margin_right  = 0.0f; // Width if min_x == max_x
    float margin_bottom = 0.0f; // Height if min_y == max_y

    float padding_left   = 0.0f;
    float padding_top    = 0.0f;
    float padding_right  = 0.0f; // Width if min_x == max_x
    float padding_bottom = 0.0f; // Height if min_y == max_y

    Vec2 preferred_size = { 0.0f, 0.0f };

  protected:
    Rect m_local_rect  = { 0, 0, 0, 0 };
    Rect m_global_rect = { 0, 0, 0, 0 };

    std::weak_ptr<UINode>                m_parent;
    std::vector<std::shared_ptr<UINode>> m_children;

  public:
    UINode() { set_layout_strategy(std::make_unique<DefaultLayout>()); }
    virtual ~UINode() = default; //

    void add_child(std::shared_ptr<UINode> child) {
        child->m_parent = shared_from_this();
        m_children.push_back(child);
    }
    std::vector<std::shared_ptr<UINode>> get_children() const {
        return m_children;
    }

    std::weak_ptr<UINode> get_parent() const { return m_parent; }

    void
    set_layout_strategy(std::unique_ptr<ILayoutStrategy> new_layout_strategy) {
        layout_strategy = std::move(new_layout_strategy);
    }

    Rect get_global_rect() const { return m_global_rect; }
    void set_global_rect(Rect rect) { m_global_rect = rect; }

    // node.hpp
    virtual void measure_layout(LayoutConstraint parent_constraint) {

        // Let's calculate our new constraint
        LayoutConstraint new_layout_constraint = parent_constraint;

        // screen budget for children must shrink due to padding taking
        // internal space
        new_layout_constraint.max_width -= padding_left + padding_right;
        new_layout_constraint.max_height -= padding_top + padding_bottom;

        // ensure it's not negative
        new_layout_constraint.max_width =
            std::max(new_layout_constraint.max_width, 0.0f);
        new_layout_constraint.max_height =
            std::max(new_layout_constraint.max_height, 0.0f);

        // Layout manager, decide size to make children work, here's screen
        // budget.
        if (!m_children.empty()) {
            preferred_size =
                layout_strategy->measure(m_children, new_layout_constraint);
        } else {
            // simple override for texture, labels and buttons
            // will return 0 size if not overridden
            preferred_size = get_content_size();
        }

        // I need my padding back
        preferred_size.x += padding_left + padding_right;
        preferred_size.y += padding_top + padding_bottom;

        // Finally make sure my parent is happy and done
        preferred_size.x = std::clamp(
            preferred_size.x,
            layout_constraint.min_width,
            std::min(parent_constraint.max_width, layout_constraint.max_width)
        );
        preferred_size.y = std::clamp(
            preferred_size.y,
            layout_constraint.min_height,
            std::min(parent_constraint.max_height, layout_constraint.max_height)
        );
    }

    virtual Vec2 get_content_size() const { return Vec2::ZERO(); }

    virtual void update_layout(Rect allocated_space) {

        set_global_rect(
            { allocated_space.x - margin_left,
              allocated_space.y - margin_top,
              allocated_space.width + margin_right,
              allocated_space.height + margin_bottom }
        );

        Rect new_rect = {
            allocated_space.x + padding_left,
            allocated_space.y + padding_top,
            allocated_space.width - (padding_right + padding_left),
            allocated_space.height - (padding_bottom + padding_top)
        };

        if (!m_children.empty()) {
            layout_strategy->arrange(m_children, new_rect);
        }

        // float anchor_left = parent_rect.x + (parent_rect.width *
        // anchor.min_x); float anchor_top  = parent_rect.y +
        // (parent_rect.height * anchor.min_y); float anchor_right =
        // parent_rect.x + (parent_rect.width * anchor.max_x); float
        // anchor_bottom =
        //     parent_rect.y + (parent_rect.height * anchor.max_y);
        //
        // // Determine final screen bounds
        // if (anchor.min_x == anchor.max_x) {
        //     // Point anchor: width comes from offset_right, position adjusted
        //     by
        //     // pivot
        //     m_global_rect.width = offset_right;
        //     m_global_rect.x =
        //         anchor_left + offset_left - (m_global_rect.width * pivot.x);
        // } else {
        //     // Stretch anchor: edges are pinned to parent anchors
        //     m_global_rect.x = anchor_left + offset_left;
        //     m_global_rect.width =
        //         (anchor_right - offset_right) - m_global_rect.x;
        // }
        //
        // if (anchor.min_y == anchor.max_y) {
        //     m_global_rect.height = offset_bottom;
        //     m_global_rect.y =
        //         anchor_top + offset_top - (m_global_rect.height * pivot.y);
        // } else {
        //     m_global_rect.y = anchor_top + offset_top;
        //     m_global_rect.height =
        //         (anchor_bottom - offset_bottom) - m_global_rect.y;
        // }
        //
        // Recursively update children using this node's new global rect
    }

    // 2. INPUT PASS: Children to parent
    virtual bool process_input(Vec2 mouse_pos) {
        if (!visible || !active)
            return false;

        // Process children in reverse order (topmost rendered elements get
        // input first)
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
            if ((*it)->process_input(mouse_pos)) {
                return true; // Input was consumed by a child!
            }
        }
        return false;
    }

    // 3. RENDER PASS: Draw back-to-front
    virtual void draw() const {
        if (!visible)
            return;

        if (self_visible)
            draw_self();

        for (auto& child : m_children) {
            child->draw();
        }
    }

  protected:
    virtual void draw_self() const {}
};

class UIUtil {};

} // namespace varicle::ui

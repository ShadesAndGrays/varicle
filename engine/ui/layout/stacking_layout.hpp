#pragma once

#include "base_layout.hpp"

namespace varicle::ui {
// Base Stacking
class StackingLayout : public ILayoutStrategy {
  protected:
    float m_gap                           = 0.0f;
    // Avoid instantiation 
    StackingLayout()                      = default;
    StackingLayout(const StackingLayout&) = default;

  public:
    void  set_gap(float gap) { m_gap = gap; }
    float get_gap() { return m_gap; }
};

} // namespace varicle::ui

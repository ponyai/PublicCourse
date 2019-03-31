// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/layer/lane_layer.h"
#include "common/utils/display/color.h"

namespace utils {
namespace display {

void LaneLayer::Draw() const {
  static_cast<MapOpenglPainter*>(gl_painter_)
      ->DrawHighlightedLanes(data_, utils::display::Color::Red());
}

}  // namespace display
}  // namespace utils

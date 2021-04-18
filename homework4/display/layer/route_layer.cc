// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/layer/route_layer.h"
#include "common/utils/display/color.h"
#include "common/utils/math/vec3d.h"

namespace utils {
namespace display {

void RouteLayer::Draw() const {
  static_cast<MapOpenglPainter*>(gl_painter_)->DrawRoute(data_, user_interface_data_);
}

}  // namespace display
}  // namespace utils

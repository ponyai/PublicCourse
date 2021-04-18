// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common/utils/display/rendering_controllers.h"
#include "common/utils/math/vec2d.h"

namespace utils {
namespace display {

struct UserInterfaceData {
  // Lane pick.
  bool has_picked_point = false;
  math::Vec2d picked_point;
  std::string picked_nearest_lane_id;

  utils::display::CameraController* camera_controller = nullptr;  // Not owned.
  std::unordered_map<std::string, bool> layers_settings;
};

}  // namespace display
}  // namespace utils

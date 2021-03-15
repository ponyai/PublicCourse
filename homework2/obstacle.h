// Copyright @2019 Pony AI Inc. All rights reserved.

#pragma once

#include <numeric>
#include <string>
#include <vector>

#include "common/utils/math/vec2d.h"

struct Obstacle {
  std::string id;
  std::vector<math::Vec2d> polygon;
  double floor = std::numeric_limits<double>::infinity();
  double ceiling = -std::numeric_limits<double>::infinity();
};
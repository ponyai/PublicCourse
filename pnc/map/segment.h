// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/math/vec2d.h"

namespace pnc {
namespace map {

class Segment {
 public:
  Segment(const math::Vec2d& start, const math::Vec2d& end) : start_(start), end_(end) {
    const double dx = end_.x - start_.x;
    const double dy = end_.y - start_.y;
    length_ = hypot(dx, dy);
    if (length_ <= math::kEpsilon) {
      unit_direction_ = math::Vec2d(0, 0);
    } else {
      unit_direction_ = math::Vec2d(dx / length_, dy / length_);
    }
  }

  double DistanceToPoint(const math::Vec2d& point) const {
    if (length_ <= math::kEpsilon) {
      return point.DistanceToPoint(start_);
    }
    const double x0 = point.x - start_.x;
    const double y0 = point.y - start_.y;
    const double proj = x0 * unit_direction_.x + y0 * unit_direction_.y;
    if (proj <= 0.0) {
      return hypot(x0, y0);
    }
    if (proj >= length_) {
      return point.DistanceToPoint(end_);
    }
    return std::abs(x0 * unit_direction_.y - y0 * unit_direction_.x);
  }

 private:
  math::Vec2d start_;
  math::Vec2d end_;
  math::Vec2d unit_direction_;
  double length_ = 0.0;
};

}  // namespace map
}  // namespace pnc

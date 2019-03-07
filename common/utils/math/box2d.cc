// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/math/box2d.h"

#include <algorithm>
#include <glog/logging.h>
#include <limits>
#include <sstream>

#include "common/utils/math/math_utils.h"
#include "common/utils/math/vec2d.h"

namespace math {

Box2d::Box2d(const Vec2d& center, double heading, double length, double width)
    : center_(center),
      cos_heading_(std::cos(heading)),
      sin_heading_(std::sin(heading)),
      length_(length),
      width_(width) {
  Init();
}

Box2d::Box2d(const Vec2d& center, const Vec2d& unit_direction, double length, double width)
    : center_(center),
      cos_heading_(unit_direction.x),
      sin_heading_(unit_direction.y),
      length_(length),
      width_(width) {
  Init();
}

void Box2d::Init() {
  CHECK_GE(length_, -kEpsilon);
  CHECK_GE(width_, -kEpsilon);
  length_ = std::max(0.0, length_);
  width_ = std::max(0.0, width_);

  half_length_ = length_ / 2.0;
  half_width_ = width_ / 2.0;

  const double abs_cos_heading = std::abs(cos_heading_);
  const double abs_sin_heading = std::abs(sin_heading_);
  const double dx = abs_cos_heading * half_length_ + abs_sin_heading * half_width_;
  const double dy = abs_cos_heading * half_width_ + abs_sin_heading * half_length_;
  min_x_ = center_.x - dx;
  max_x_ = center_.x + dx;
  min_y_ = center_.y - dy;
  max_y_ = center_.y + dy;
}

Box2d Box2d::FromAABox(const Vec2d& one_corner, const Vec2d& opposite_corner) {
  double x1 = one_corner.x;
  double x2 = opposite_corner.x;
  if (x1 > x2) {
    std::swap(x1, x2);
  }
  double y1 = one_corner.y;
  double y2 = opposite_corner.y;
  if (y1 > y2) {
    std::swap(y1, y2);
  }
  return Box2d({(x1 + x2) / 2.0, (y1 + y2) / 2.0}, {1.0, 0.0}, x2 - x1, y2 - y1);
}

void Box2d::Normalize() {
  if (length_ < width_) {
    std::swap(length_, width_);
    double t = cos_heading_;
    cos_heading_ = sin_heading_;
    sin_heading_ = -t;
  }
  if (sin_heading_ < 0.0) {
    cos_heading_ = -cos_heading_;
    sin_heading_ = -sin_heading_;
  }
  Init();
}

std::vector<Vec2d> Box2d::GetCorners() const {
  std::vector<Vec2d> corners;
  corners.reserve(4);
  const double dx1 = half_length_ * cos_heading_;
  const double dy1 = half_length_ * sin_heading_;
  const double dx2 = -half_width_ * sin_heading_;
  const double dy2 = half_width_ * cos_heading_;
  corners.emplace_back(center_.x + dx1 + dx2, center_.y + dy1 + dy2);
  corners.emplace_back(center_.x - dx1 + dx2, center_.y - dy1 + dy2);
  corners.emplace_back(center_.x - dx1 - dx2, center_.y - dy1 - dy2);
  corners.emplace_back(center_.x + dx1 - dx2, center_.y + dy1 - dy2);
  return corners;
}

Vec2d Box2d::FrontLeftCorner() const {
  const double dx1 = half_length_ * cos_heading_;
  const double dy1 = half_length_ * sin_heading_;
  const double dx2 = -half_width_ * sin_heading_;
  const double dy2 = half_width_ * cos_heading_;
  return Vec2d(center_.x + dx1 + dx2, center_.y + dy1 + dy2);
}

Vec2d Box2d::FrontRightCorner() const {
  const double dx1 = half_length_ * cos_heading_;
  const double dy1 = half_length_ * sin_heading_;
  const double dx2 = -half_width_ * sin_heading_;
  const double dy2 = half_width_ * cos_heading_;
  return Vec2d(center_.x + dx1 - dx2, center_.y + dy1 - dy2);
}

Vec2d Box2d::BackLeftCorner() const {
  const double dx1 = half_length_ * cos_heading_;
  const double dy1 = half_length_ * sin_heading_;
  const double dx2 = -half_width_ * sin_heading_;
  const double dy2 = half_width_ * cos_heading_;
  return Vec2d(center_.x - dx1 + dx2, center_.y - dy1 + dy2);
}

Vec2d Box2d::BackRightCorner() const {
  const double dx1 = half_length_ * cos_heading_;
  const double dy1 = half_length_ * sin_heading_;
  const double dx2 = -half_width_ * sin_heading_;
  const double dy2 = half_width_ * cos_heading_;
  return Vec2d(center_.x - dx1 - dx2, center_.y - dy1 - dy2);
}

std::string Box2d::ToString() const {
  std::ostringstream sout;
  sout << "Box2d( center = " << center_.ToString()
       << ", heading = " << atan2(sin_heading_, cos_heading_) << ", length = " << length_
       << ", width = " << width_ << " )";
  sout.flush();
  return sout.str();
}

double Box2d::DistanceToPoint(const Vec2d& point) const {
  return DistanceToPoint(point.x, point.y);
}

double Box2d::DistanceToPoint(double x, double y) const {
  x -= center_.x;
  y -= center_.y;
  const double abs_proj_axis = std::abs(x * cos_heading_ + y * sin_heading_);
  const double abs_prod_axis = std::abs(x * sin_heading_ - y * cos_heading_);
  return std::hypot(std::max(0.0, abs_proj_axis - half_length_),
                    std::max(0.0, abs_prod_axis - half_width_));
}

double Box2d::DistanceSqrToPoint(const Vec2d& point) const {
  return DistanceSqrToPoint(point.x, point.y);
}

double Box2d::DistanceSqrToPoint(double x, double y) const {
  x -= center_.x;
  y -= center_.y;
  const double abs_proj_axis = std::abs(x * cos_heading_ + y * sin_heading_);
  const double abs_prod_axis = std::abs(x * sin_heading_ - y * cos_heading_);
  double distance_sqr = 0.0;
  const double d1 = abs_proj_axis - half_length_;
  if (d1 > 0.0) {
    distance_sqr += Sqr(d1);
  }
  const double d2 = abs_prod_axis - half_width_;
  if (d2 > 0.0) {
    distance_sqr += Sqr(d2);
  }
  return distance_sqr;
}

bool Box2d::IsPointIn(const Vec2d& point) const { return IsPointIn(point.x, point.y); }

bool Box2d::IsPointIn(double x, double y) const {
  x -= center_.x;
  y -= center_.y;
  return std::abs(x * cos_heading_ + y * sin_heading_) < half_length_ &&
         std::abs(x * sin_heading_ - y * cos_heading_) < half_width_;
}

bool Box2d::IsPointOnBoundary(const Vec2d& point) const {
  return IsPointOnBoundary(point.x, point.y);
}

bool Box2d::IsPointOnBoundary(double x, double y) const {
  x -= center_.x;
  y -= center_.y;
  const double dx = std::abs(x * cos_heading_ + y * sin_heading_) - half_length_;
  const double dy = std::abs(x * sin_heading_ - y * cos_heading_) - half_width_;
  return (std::abs(dx) <= kEpsilon && dy <= kEpsilon) ||
         (std::abs(dy) <= kEpsilon && dx <= kEpsilon);
}

bool Box2d::IsPointInOrOnBoundary(const Vec2d& point) const {
  return IsPointInOrOnBoundary(point.x, point.y);
}

bool Box2d::IsPointInOrOnBoundary(double x, double y) const {
  x -= center_.x;
  y -= center_.y;
  return std::abs(x * cos_heading_ + y * sin_heading_) <= half_length_ + kEpsilon &&
         std::abs(x * sin_heading_ - y * cos_heading_) <= half_width_ + kEpsilon;
}

bool Box2d::HasOverlapWithBox(const Box2d& box) const {
  const double dx = box.center().x - center_.x;
  const double dy = box.center().y - center_.y;
  const double abs_inner_prod =
      std::abs(cos_heading_ * box.cos_heading() + sin_heading_ * box.sin_heading());
  const double abs_outer_prod =
      std::abs(cos_heading_ * box.sin_heading() - sin_heading_ * box.cos_heading());

  return std::abs(dx * cos_heading_ + dy * sin_heading_) <= half_length_ +
                                                                abs_inner_prod * box.half_length() +
                                                                abs_outer_prod * box.half_width() &&
         std::abs(dx * sin_heading_ - dy * cos_heading_) <=
             half_width_ + abs_outer_prod * box.half_length() + abs_inner_prod * box.half_width() &&
         std::abs(dx * box.cos_heading() + dy * box.sin_heading()) <=
             box.half_length() + abs_inner_prod * half_length_ + abs_outer_prod * half_width_ &&
         std::abs(dx * box.sin_heading() - dy * box.cos_heading()) <=
             box.half_width() + abs_outer_prod * half_length_ + abs_inner_prod * half_width_;
}

Vec2d Box2d::HeadingVector() const { return Vec2d(cos_heading_, sin_heading_); }

Box2d operator+(const Box2d& box, const Vec2d& offset) {
  return Box2d(box.center() + offset, box.HeadingVector(), box.length(), box.width());
}

}  // namespace math

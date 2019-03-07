// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

// #include "common/utils/math/segment2d.h"
#include "common/utils/math/vec2d.h"

namespace math {

// Presentation of one rectangle in 2D space.
class Box2d {
 public:
  Box2d() = default;
  Box2d(const Vec2d& center, double heading, double length, double width);
  Box2d(const Vec2d& center, const Vec2d& unit_direction, double length, double width);

  static Box2d FromAABox(const Vec2d& one_corner, const Vec2d& opposite_corner);

  // Normalize by length >= width, and 0 <= angle < M_PI.
  void Normalize();

  const Vec2d& center() const { return center_; }
  double cos_heading() const { return cos_heading_; }
  double sin_heading() const { return sin_heading_; }
  Vec2d HeadingVector() const;
  double length() const { return length_; }
  double width() const { return width_; }
  double area() const { return length_ * width_; }
  double half_length() const { return half_length_; }
  double half_width() const { return half_width_; }

  double min_x() const { return min_x_; }
  double min_y() const { return min_y_; }
  double max_x() const { return max_x_; }
  double max_y() const { return max_y_; }
  double heading() const { return std::atan2(sin_heading(), cos_heading()); }

  std::string ToString() const;

  // Get four corners in ccw order: front-left, back-left, back-right, front-right.
  std::vector<Vec2d> GetCorners() const;
  Vec2d FrontLeftCorner() const;
  Vec2d FrontRightCorner() const;
  Vec2d BackLeftCorner() const;
  Vec2d BackRightCorner() const;

  double DistanceToPoint(const Vec2d& point) const;
  double DistanceToPoint(double x, double y) const;
  double DistanceSqrToPoint(const Vec2d& point) const;
  double DistanceSqrToPoint(double x, double y) const;

  bool IsPointIn(const Vec2d& point) const;
  bool IsPointIn(double x, double y) const;
  bool IsPointOnBoundary(const Vec2d& point) const;
  bool IsPointOnBoundary(double x, double y) const;
  bool IsPointInOrOnBoundary(const Vec2d& point) const;
  bool IsPointInOrOnBoundary(double x, double y) const;

  bool HasOverlapWithBox(const Box2d& box) const;

 private:
  void Init();

  Vec2d center_;
  double cos_heading_ = 1.0;
  double sin_heading_ = 0.0;
  double length_ = 0.0;
  double width_ = 0.0;

  // Used to improve the speed in distance computation.
  double half_length_ = 0.0;
  double half_width_ = 0.0;
  double min_x_ = 0.0;
  double max_x_ = 0.0;
  double min_y_ = 0.0;
  double max_y_ = 0.0;

  // Shallow copy and move are OK.
};

Box2d operator+(const Box2d& box, const Vec2d& offset);

}  // namespace math

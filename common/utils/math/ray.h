// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

namespace math {

template <typename T>
struct Ray {
  T starting_point;
  T direction;

  Ray() = default;
  Ray(const T& s_point, const T& dir) : starting_point(s_point), direction(dir) {}
  ~Ray() = default;
  std::string ToString() const;
};

template <>
inline std::string Ray<Vec2d>::ToString() const {
  std::ostringstream sout;
  sout << "Ray2d( starting_point = " << starting_point.ToString()
       << ", direction = " << direction.ToString() << " )";
  return sout.str();
}

template <>
inline std::string Ray<Vec3d>::ToString() const {
  std::ostringstream sout;
  sout << "Ray3d( starting_point = " << starting_point.ToString()
       << ", direction = " << direction.ToString() << " )";
  return sout.str();
}

using Ray2d = Ray<Vec2d>;
using Ray3d = Ray<Vec3d>;

}  // namespace math

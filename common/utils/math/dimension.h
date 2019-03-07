// Copyright @2018 Pony AI Inc. All rights reserved.
#pragma once

#include "Eigen/Core"
#include "glm/glm.hpp"

#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

namespace math {

// Type traits for dimension of vector template.
template <typename T>
struct Dimension;

template <>
struct Dimension<float> {
  using ElemType = float;
  static constexpr int kValue = 1;
};

template <>
struct Dimension<double> {
  using ElemType = double;
  static constexpr int kValue = 1;
};

template <typename T>
struct Dimension<math::Vec2<T>> {
  using ElemType = T;
  static constexpr int kValue = 2;
};

template <typename T>
struct Dimension<math::Vec3<T>> {
  using ElemType = T;
  static constexpr int kValue = 3;
};

// Specialize for Eigen::Vector2X.
template <typename T>
struct Dimension<Eigen::Matrix<T, 2, 1>> {
  using ElemType = T;
  static constexpr int kValue = 2;
};

// Specialize for Eigen::Vector3X.
template <typename T>
struct Dimension<Eigen::Matrix<T, 3, 1>> {
  using ElemType = T;
  static constexpr int kValue = 3;
};

template <typename T>
struct Dimension<glm::tvec2<T>> {
  using ElemType = T;
  static constexpr int kValue = 2;
};

template <typename T>
struct Dimension<glm::tvec3<T>> {
  using ElemType = T;
  static constexpr int kValue = 3;
};

}  // namespace math

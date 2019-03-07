// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "common/utils/containers/array_view.h"
#include "common/utils/containers/fixed_array.h"
#include "common/utils/display/color.h"
#include "common/utils/gl/context.h"
#include "common/utils/math/dimension.h"
#include "common/utils/math/vec3d.h"

namespace gl {
namespace geometry_utils {

constexpr double kUtilsEpsilon = 1e-6;

template <typename DataType>
constexpr GLenum GetGLFloatType() {
  return std::is_same<typename math::Dimension<DataType>::ElemType, float>::value ? GL_FLOAT
                                                                                  : GL_DOUBLE;
}

template <typename DataType>
constexpr int GetDimension() {
  return math::Dimension<DataType>::kValue;
}

template <>
constexpr GLenum GetGLFloatType<utils::display::Color>() {
  return std::is_same<typename utils::display::Color::ElemType, float>::value ? GL_FLOAT
                                                                              : GL_DOUBLE;
}

template <>
constexpr int GetDimension<utils::display::Color>() {
  return utils::display::Color::kValue;
}

// It seems that glm::vec2, glm::vec3, glm::vec4 don't have a common parent class, we have to
// implement the specializations for the template function separately.
template <>
constexpr GLenum GetGLFloatType<glm::vec2>() {
  return GL_FLOAT;
}

template <>
constexpr int GetDimension<glm::vec2>() {
  return 2;
}

template <>
constexpr GLenum GetGLFloatType<glm::vec3>() {
  return GL_FLOAT;
}

template <>
constexpr int GetDimension<glm::vec3>() {
  return 3;
}

template <>
constexpr GLenum GetGLFloatType<glm::vec4>() {
  return GL_FLOAT;
}

template <>
constexpr int GetDimension<glm::vec4>() {
  return 4;
}

template <typename DataType>
utils::FixedArray<DataType> ReconstructArrayByIndex(const utils::ConstArrayView<DataType>& array,
                                                    const std::vector<int>& indices) {
  const int n = indices.size();
  utils::FixedArray<DataType> new_array(n);
  for (int i = 0; i < n; ++i) {
    new_array[i] = array[indices[i]];
  }
  return new_array;
}

bool IsZero(double x);

// Check whether the given point is in the triangle. Please make sure that there're no 3 points in
// the same line first.
bool IsPointInTriangle(const math::Vec3d& p, const math::Vec3d& s1, const math::Vec3d& s2,
                       const math::Vec3d& s3);

// OpenGL doesn't draw polygon perfectly, we triangulate it first. This functions can only handle
// simple polygons which means the input SHOULDN'T be self-intersecting. You can find more details
// on the triangulation algorithm here:
//    https://en.wikipedia.org/wiki/Polygon_triangulation#Ear_clipping_method
//
// Noting that this is a simple yet timetime-consuming implementation( O(V^3) ), NEVER run it on
// polygons who have more than 100 vertices.
void EarClippingTriangulation(const utils::ConstArrayView<math::Vec3d>& points,
                              std::vector<int>* triangles_indices_ptr);

}  // namespace geometry_utils
}  // namespace gl

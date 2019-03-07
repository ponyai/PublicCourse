// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cmath>
#include <limits>
#include <sstream>
#include <string>

#include <glog/logging.h>
#include <Eigen/Core>

#include "common/proto/geometry.pb.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/math/common.h"
#include "common/utils/math/math_utils.h"
#include "common/utils/math/vec2d.h"

namespace math {

// Presentation of points in 3D space.
template <typename T>
struct Vec3 {
 public:
  static_assert(std::is_floating_point<T>::value,
                "Template Vec3<> only supports floating-point types.");

  using value_type = T;

  Vec3() : Vec3(0.0, 0.0, 0.0) {}
  Vec3(T x, T y, T z);
  Vec3(const Vec2<T>& xy, T z) : x(xy.x), y(xy.y), z(z) {}
  explicit Vec3(const Eigen::Matrix<T, 3, 1>& v) : x(v(0)), y(v(1)), z(v(2)) {}

  T LengthSqr() const;
  T Length() const;

  void Normalize();

  Vec3 OuterProd(const Vec3& other_vec) const;
  T InnerProd(const Vec3& other_vec) const;

  T DistanceToPoint(const Vec3& point) const;
  T DistanceSqrToPoint(const Vec3& point) const;

  std::string ToString() const;

  T operator[](int index) const;
  T& operator[](int index);
  Vec3 operator+(const Vec3& point) const;
  Vec3 operator-(const Vec3& point) const;
  Vec3& operator+=(const Vec3& point);
  Vec2<T> xy() const;

  union {
    struct {
      T x;
      T y;
      T z;
    };
    T elements[3] = {T(0)};
  };

  // Shallow copy and move are OK.
};

static_assert(sizeof(Vec3<double>) == sizeof(double) * 3, "Vec3<double> should be compact.");
static_assert(sizeof(Vec3<float>) == sizeof(float) * 3, "Vec3<float> should be compact.");

template <typename T>
Vec3<T> operator*(const Vec3<T>& point, T ratio);

template <typename T>
Vec3<T> operator*(T ratio, const Vec3<T>& point);

template <typename T>
Vec3<T> operator/(const Vec3<T>& point, T ratio);

template <typename T>
Vec3<T> operator-(const Vec3<T>& point);

template <class PointType>
Vec3<double> ToVec3d(const PointType& point) {
  return Vec3<double>(point.x(), point.y(), point.z());
}

using Vec3d = Vec3<double>;

// =================================== Implementation ===================================

template <typename T>
inline Vec3<T>::Vec3(T x, T y, T z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

template <typename T>
inline T Vec3<T>::operator[](int index) const {
  DCHECK(index >= 0 && index < 3);
  return elements[index];
}

template <typename T>
T& Vec3<T>::operator[](int index) {
  DCHECK(index >= 0 && index < 3);
  return elements[index];
}

template <typename T>
inline T Vec3<T>::LengthSqr() const {
  return Sqr(x) + Sqr(y) + Sqr(z);
}

template <typename T>
inline T Vec3<T>::Length() const {
  return std::sqrt(LengthSqr());
}

template <typename T>
inline void Vec3<T>::Normalize() {
  const T l = Length();
  if (l > kEpsilon) {
    x /= l;
    y /= l;
    z /= l;
  }
}

template <typename T>
inline T Vec3<T>::DistanceToPoint(const Vec3<T>& point) const {
  return (*this - point).Length();
}

template <typename T>
inline T Vec3<T>::DistanceSqrToPoint(const Vec3<T>& point) const {
  return (*this - point).LengthSqr();
}

template <typename T>
inline Vec3<T> Vec3<T>::OuterProd(const Vec3<T>& other_vec) const {
  Vec3<T> result;
  result.x = y * other_vec.z - z * other_vec.y;
  result.y = z * other_vec.x - x * other_vec.z;
  result.z = x * other_vec.y - y * other_vec.x;
  return result;
}

template <typename T>
inline T Vec3<T>::InnerProd(const Vec3<T>& other_vec) const {
  return x * other_vec.x + y * other_vec.y + z * other_vec.z;
}

template <typename T>
inline std::string Vec3<T>::ToString() const {
  std::ostringstream sout;
  sout << "Vec3d( x = " << x << ", y = " << y << ", z = " << z << " )";
  return sout.str();
}

template <typename T>
inline Vec3<T>& Vec3<T>::operator+=(const Vec3& point) {
  x += point.x;
  y += point.y;
  z += point.z;
  return *this;
}

template <typename T>
inline Vec2<T> Vec3<T>::xy() const {
  return Vec2<T>(x, y);
}

template <typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3& point) const {
  return Vec3<T>(x + point.x, y + point.y, z + point.z);
}

template <typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3& point) const {
  return Vec3<T>(x - point.x, y - point.y, z - point.z);
}

template <typename T>
inline Vec3<T> operator*(const Vec3<T>& point, T ratio) {
  return Vec3<T>(point.x * ratio, point.y * ratio, point.z * ratio);
}

template <typename T>
inline Vec3<T> operator*(T ratio, const Vec3<T>& point) {
  return Vec3<T>(point.x * ratio, point.y * ratio, point.z * ratio);
}

template <typename T>
inline Vec3<T> operator/(const Vec3<T>& point, T ratio) {
  if (std::abs(ratio) <= std::numeric_limits<T>::epsilon()) {
    return Vec3<T>(0, 0, 0);
  }
  return Vec3<T>(point.x / ratio, point.y / ratio, point.z / ratio);
}

template <typename T>
inline Vec3<T> operator-(const Vec3<T>& point) {
  return Vec3<T>(-point.x, -point.y, -point.z);
}

inline interface::geometry::Point3D ToProto(const Vec3d& point) {
  interface::geometry::Point3D proto;
  proto.set_x(point.x);
  proto.set_y(point.y);
  proto.set_z(point.z);
  return proto;
}

template <typename T>
Eigen::Matrix<T, 3, 1> ToMatrix(const Vec3<T>& point) {
  return Eigen::Matrix<T, 3, 1>(point.x, point.y, point.z);
}

const Eigen::Matrix3Xd ToMatrix(const utils::ConstArrayView<Vec3d>& points);

}  // namespace math

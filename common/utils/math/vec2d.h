// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>

#include <Eigen/Core>

#include "common/utils/containers/array_view.h"
#include "common/utils/math/common.h"

namespace math {

// Presentation of points in 2D space.
template <typename T>
struct Vec2 {
 public:
  static_assert(std::is_floating_point<T>::value,
                "Template Vec2<> only supports floating-point types.");

  using value_type = T;

  Vec2() = default;
  Vec2(T x, T y);
  explicit Vec2(const Eigen::Matrix<T, 2, 1>& v) : x(v(0)), y(v(1)) {}

  static Vec2 FromUnit(T heading);
  static Vec2 FromLengthAndHeading(T length, T heading);
  Vec2 Rotate(const double& angle) const;
  Vec2 Rotate90() const;
  Vec2 Rotate270() const;

  T LengthSqr() const;
  T Length() const;

  T Angle() const;
  T ApproximateAngle() const;
  void Normalize();

  T InnerProd(const Vec2& other_vec) const;
  T OuterProd(const Vec2& other_vec) const;

  T DistanceToPoint(const Vec2& point) const;
  T DistanceToPoint(T point_x, T point_y) const;
  T DistanceSqrToPoint(const Vec2& point) const;
  T DistanceSqrToPoint(T point_x, T point_y) const;
  T DistanceToLine(const Vec2& p0, const Vec2& p1) const;

  std::string ToString() const;

  Vec2 operator+(const Vec2& point) const;
  Vec2 operator-(const Vec2& point) const;
  Vec2& operator+=(const Vec2& point);
  Vec2& operator-=(const Vec2& point);
  Vec2& operator*=(T ratio);
  Vec2& operator/=(T ratio);
  bool operator==(const Vec2& point) const;

  T x = 0;
  T y = 0;

  // Shallow copy and move are OK.
};

template <typename T>
Vec2<T> operator*(const Vec2<T>& point, T ratio);

template <typename T>
Vec2<T> operator*(T ratio, const Vec2<T>& point);

template <typename T>
Vec2<T> operator/(const Vec2<T>& point, T ratio);

template <typename T>
Vec2<T> operator-(const Vec2<T>& point);

template <class PointType>
Vec2<double> ToVec2d(const PointType& point) {
  return Vec2<double>(point.x(), point.y());
}

template <typename T>
T OuterProd(const Vec2<T>& reference, const Vec2<T>& point_a, const Vec2<T>& point_b);

template <typename T>
T InnerProd(const Vec2<T>& reference, const Vec2<T>& point_a, const Vec2<T>& point_b);

using Vec2d = Vec2<double>;

template <typename T>
T Sqr(T x);

inline double Lerp(double start, double end, double ratio);

template <typename T>
Vec2<T> Lerp(const Vec2<T>& start, const Vec2<T>& end, double ratio);

// =================================== Implementation ===================================

template <typename T>
inline Vec2<T>::Vec2(T x, T y) : x(x), y(y) {}

template <typename T>
inline Vec2<T> Vec2<T>::FromUnit(T heading) {
  return Vec2(std::cos(heading), std::sin(heading));
}

template <typename T>
inline Vec2<T> Vec2<T>::FromLengthAndHeading(T length, T heading) {
  return Vec2(length * std::cos(heading), length * std::sin(heading));
}

template <typename T>
inline Vec2<T> Vec2<T>::Rotate(const double& angle) const {
  T rotate_x = std::cos(angle) * x - std::sin(angle) * y;
  T rotate_y = std::sin(angle) * x + std::cos(angle) * y;
  return Vec2(rotate_x, rotate_y);
}

template <typename T>
inline Vec2<T> Vec2<T>::Rotate90() const {
  return Vec2(-y, x);
}

template <typename T>
inline Vec2<T> Vec2<T>::Rotate270() const {
  return Vec2(y, -x);
}

template <typename T>
inline T Vec2<T>::LengthSqr() const {
  return Sqr(x) + Sqr(y);
}

template <typename T>
inline T Vec2<T>::Length() const {
  return std::hypot(x, y);
}

template <typename T>
inline T Vec2<T>::Angle() const {
  return std::atan2(y, x);
}

template <typename T>
inline T Vec2<T>::ApproximateAngle() const {
  return ApproximateAtan2(y, x);
}

template <typename T>
inline void Vec2<T>::Normalize() {
  const T l = Length();
  if (l > math::kEpsilon) {
    x /= l;
    y /= l;
  }
}

template <typename T>
inline T Vec2<T>::DistanceToPoint(const Vec2<T>& point) const {
  return std::hypot(x - point.x, y - point.y);
}

template <typename T>
inline T Vec2<T>::DistanceToPoint(T point_x, T point_y) const {
  return std::hypot(x - point_x, y - point_y);
}

template <typename T>
inline T Vec2<T>::DistanceSqrToPoint(const Vec2<T>& point) const {
  return Sqr(x - point.x) + Sqr(y - point.y);
}

template <typename T>
inline T Vec2<T>::DistanceSqrToPoint(T point_x, T point_y) const {
  return Sqr(x - point_x) + Sqr(y - point_y);
}

template <typename T>
inline T Vec2<T>::InnerProd(const Vec2<T>& other_vec) const {
  return x * other_vec.x + y * other_vec.y;
}

template <typename T>
inline T Vec2<T>::OuterProd(const Vec2<T>& other_vec) const {
  return x * other_vec.y - y * other_vec.x;
}

template <typename T>
inline std::string Vec2<T>::ToString() const {
  std::ostringstream sout;
  sout << "Vec2d( x = " << x << ", y = " << y << " )";
  return sout.str();
}

template <typename T>
inline T Vec2<T>::DistanceToLine(const Vec2& p0, const Vec2& p1) const {
  return std::abs(math::OuterProd(*this, p0, p1)) / p0.DistanceToPoint(p1);
}

template <typename T>
inline Vec2<T>& Vec2<T>::operator+=(const Vec2& point) {
  x += point.x;
  y += point.y;
  return *this;
}

template <typename T>
inline Vec2<T>& Vec2<T>::operator-=(const Vec2& point) {
  x -= point.x;
  y -= point.y;
  return *this;
}

template <typename T>
inline Vec2<T>& Vec2<T>::operator*=(T ratio) {
  x *= ratio;
  y *= ratio;
  return *this;
}

template <typename T>
inline Vec2<T>& Vec2<T>::operator/=(T ratio) {
  x /= ratio;
  y /= ratio;
  return *this;
}

template <typename T>
inline Vec2<T> Vec2<T>::operator+(const Vec2& point) const {
  return Vec2<T>(x + point.x, y + point.y);
}

template <typename T>
inline Vec2<T> Vec2<T>::operator-(const Vec2& point) const {
  return Vec2<T>(x - point.x, y - point.y);
}

template <typename T>
inline bool Vec2<T>::operator==(const Vec2& point) const {
  return std::fabs(x - point.x) < math::kEpsilon && std::fabs(y - point.y) < math::kEpsilon;
}

template <typename T>
inline Vec2<T> operator*(const Vec2<T>& point, T ratio) {
  return Vec2<T>(point.x * ratio, point.y * ratio);
}

template <typename T>
inline Vec2<T> operator*(T ratio, const Vec2<T>& point) {
  return Vec2<T>(point.x * ratio, point.y * ratio);
}

template <typename T>
inline Vec2<T> operator/(const Vec2<T>& point, T ratio) {
  if (std::abs(ratio) <= std::numeric_limits<T>::epsilon()) {
    return Vec2<T>(0, 0);
  }
  return Vec2<T>(point.x / ratio, point.y / ratio);
}

template <typename T>
inline Vec2<T> operator-(const Vec2<T>& point) {
  return Vec2<T>(-point.x, -point.y);
}

template <typename T>
inline T OuterProd(const Vec2<T>& reference, const Vec2<T>& point_a, const Vec2<T>& point_b) {
  return (point_a.x - reference.x) * (point_b.y - reference.y) -
         (point_a.y - reference.y) * (point_b.x - reference.x);
}

template <typename T>
inline T InnerProd(const Vec2<T>& reference, const Vec2<T>& point_a, const Vec2<T>& point_b) {
  return (point_a.x - reference.x) * (point_b.x - reference.x) +
         (point_a.y - reference.y) * (point_b.y - reference.y);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T>& point) {
  return os << point.ToString();
}

template <typename T>
Eigen::Matrix<T, 2, 1> ToMatrix(const Vec2<T>& point) {
  return Eigen::Matrix<T, 2, 1>(point.x, point.y);
}

const Eigen::Matrix2Xd ToMatrix(const utils::ConstArrayView<Vec2d>& points);

// Linear Interpolation between start and end.
template <typename T>
Vec2<T> Lerp(const Vec2<T>& start, const Vec2<T>& end, double ratio) {
  return {Lerp(start.x, end.x, ratio), Lerp(start.y, end.y, ratio)};
}

}  // namespace math

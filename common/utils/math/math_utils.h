// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "glog/logging.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

#define M_PI_f 3.14159265358979323846f
#define M_PI_2_f 1.57079632679489661923f

namespace math {

template <typename T>
struct Vec2;

template <typename T>
struct Vec3;

template <typename T>
T Sqr(T x) {
  return x * x;
}

template <typename T>
T Cube(T x) {
  return x * x * x;
}

template <typename T>
T Clamp(T value, T min_value, T max_value) {
  return std::max(min_value, std::min(max_value, value));
}

template <typename T>
T Sech(T x) {
  return static_cast<T>(2.0 / (std::exp(x) + std::exp(-x)));
}

// sinc of x radians
template <typename T>
T Sinc(T x) {
  if (std::abs(x) < kEpsilon) {
    return static_cast<T>(1);
  }
  return std::sin(x) / x;
}

template <typename T>
T Sigmoid(T x) {
  return static_cast<T>(1.0 / (1.0 + std::exp(-x)));
}

template <typename T>
T NormalPdf(T x, T sigma) {
  return std::exp(-0.5 * Sqr(x / sigma)) / (std::sqrt(2 * M_PI) * sigma);
}

template <typename T>
T NormalCdf(T mu, T sigma, T x) {
  return 0.5 * (1 + std::erf((x - mu) / (sigma * std::sqrt(2))));
}

inline int fsign(double v) { return (-math::kEpsilon < v) - (v < math::kEpsilon); }

// Returns an angle between -pi (inclusive) and pi (exclusive).
double NormalizeAngle(double angle);

// Interpolate a heading between heading_prev and heading_next, which is factor from heading_prev
// and 1 - factor from heading_next.
double InterpolateAngle(double heading_prev, double heading_next, double factor);

// Calculate the curvature on a trajectory at point current.
// The behavior is undefined if angle(prev, current, next) >= pi/2
double Curvature(const Vec2<double>& prev, const Vec2<double>& current, const Vec2<double>& next);

// Square of the curvature.
// The behavior is undefined if angle(prev, current, next) >= pi/2
double CurvatureSqr(const Vec2<double>& prev, const Vec2<double>& current,
                    const Vec2<double>& next);

// Signed curvature. Positive: left turn. Negative: right turn.
// The behavior is undefined if angle(prev, current, next) >= pi/2
double CurvatureSigned(const Vec2<double>& prev, const Vec2<double>& current,
                       const Vec2<double>& next);

double L1Norm(const std::vector<double>& v);
double L2Norm(const std::vector<double>& v);
double LInfinityNorm(const std::vector<double>& v);

template <typename T, typename S = T>
S Sum(const std::vector<T>& v) {
  return std::accumulate(v.begin(), v.end(), S{0});
}

template <typename T, typename S = T>
S Average(const std::vector<T>& v) {
  CHECK(!v.empty());
  return Sum<T, S>(v) / v.size();
}

template <class NumType>
inline bool IsNear(NumType expected, NumType actual, NumType epsilon) {
  return std::abs(actual - expected) < epsilon;
}

double StandardDeviation(const std::vector<double>& v, double average);
inline double StandardDeviation(const std::vector<double>& v) {
  return StandardDeviation(v, Average(v));
}

// If you want to compute min and p25 of data points in `v`, call `PercentilesInPlace(&v, {0, 25})`.
// Returned vector should contain 2 elements in this case, first element `min(v)` and second p25 of
// `v`.
// This is similar to `Percentiles()` except that the first argument containing all data points is
// taken by non-const pointer. This avoids the performance overhead of copying the whole list of
// data points, but may reorder caller's list of data points.
// parameters:
// v_ptr - pointer to a vector containing data points, it may be reordered by this function.
// percentages - sorted list of percentage numbers, each of them must be between 0 (inclusive)
//               and 100 (inclusive).
std::vector<double> PercentilesInPlace(std::vector<double>* v_ptr,
                                       const std::vector<int>& percentages);

// If you want to compute min and p25 of data points in `v`, call `Percentiles(&v, {0, 25})`.
// Returned vector should contain 2 elements in this case, first element `min(v)` and second p25 of
// `v`.
// This is similar to `PercentilesInPlace()` except that the first argument containing all data
// points is taken by value, implicitly making a copy of the whole list of data points. This incurs
// performance overhead if the number of data points is large, but it does not reorder caller's list
// of data points in any way.
// parameters:
// v - a vector containing data points.
// percentages - sorted list of percentage numbers, each of them must be between 0 (inclusive)
//               and 100 (inclusive).
inline std::vector<double> Percentiles(std::vector<double> v, const std::vector<int>& percentages) {
  return PercentilesInPlace(&v, percentages);
}

// Conversions between radian and degree angle measurements.
// The results will NOT normalized into [0, 2pi] nor [0, 360].
constexpr double DegreeToRadian(double angle_in_degree) { return angle_in_degree / 180.0 * M_PI; }

constexpr double RadianToDegree(double angle_in_radian) { return angle_in_radian / M_PI * 180.0; }

// Linear Interpolation between start and end.
inline double Lerp(double start, double end, double ratio) {
  return start * (1.0 - ratio) + end * ratio;
}

// Returns the count of trailing zero in the binary representation of a number.
inline int CountTrailingZeros(uint64_t n) {
#if __GNUC__ || __clang__
  return __builtin_ctzll(n);
#else
  return internal::Log2ByTableLookup(n & (-n));
#endif
}

// Returns the count of leading zero in the binary representation of a number.
inline int CountLeadingZeros(uint32_t n) {
#if __GNUC__ || __clang__
  return __builtin_clz(n);
#else
  int num_leading_zeros = 0;
  if (n <= 0x0000ffff) {
    num_leading_zeros += 16;
    n <<= 16;
  }
  if (n <= 0x00ffffff) {
    num_leading_zeros += 8;
    n <<= 8;
  }
  if (n <= 0x0fffffff) {
    num_leading_zeros += 4;
    n <<= 4;
  }
  if (n <= 0x3fffffff) {
    num_leading_zeros += 2;
    n <<= 2;
  }
  if (n <= 0x7fffffff) {
    num_leading_zeros++;
  }
  return num_leading_zeros;
#endif
}

// Returns the count of one(s) in the binary representation of a number.
inline int CountNumberOfOnes(uint64_t n) {
#if __GNUC__ || __clang__
  return __builtin_popcountll(n);
#else
  int count = 0;
  for (; n; n &= (n - 1)) {
    ++count;
  }
  return count;
#endif
}

// Return either floor(x) or floor(x)+1. The expected return value is x.
template <typename RealType, typename UniformRandomNumberGenerator>
int RandomRound(RealType x, UniformRandomNumberGenerator* urng) {
  static_assert(std::is_floating_point<RealType>::value,
                "template argument not a floating point type");
  std::uniform_real_distribution<RealType> dist;
  int i = std::floor(x);
  if (dist(*urng) < x - i) {
    return i + 1;
  } else {
    return i;
  }
}

}  // namespace math

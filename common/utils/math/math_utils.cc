// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/math/math_utils.h"

#include <algorithm>
#include <limits>
#include <numeric>

#include "glog/logging.h"

namespace math {
namespace {

template <typename T>
T PercentileIndex(T num_data_points, double fractional_percentage) {
  CHECK(fractional_percentage >= 0.0 && fractional_percentage <= 1.0);
  static_assert(std::is_integral<T>::value,
                "Integral type required for type T in PercentileIndex<T>.");
  CHECK_GT(num_data_points, 0);
  return static_cast<T>(std::round((num_data_points - 1) * fractional_percentage));
}

}  // namespace

double NormalizeAngle(double angle) {
  if (angle >= -M_PI && angle < M_PI) {
    return angle;
  }
  angle = std::fmod(angle, 2.0 * M_PI);
  if (angle >= M_PI) {
    angle -= 2 * M_PI;
  } else if (angle < -M_PI) {
    angle += 2 * M_PI;
  }
  return angle;
}

double InterpolateAngle(double heading_prev, double heading_next, double factor) {
  const double delta = NormalizeAngle(heading_next - heading_prev);
  return NormalizeAngle(heading_prev + factor * delta);
}

double CurvatureSigned(const Vec2d& prev, const Vec2d& current, const Vec2d& next) {
  auto v = (next - prev) * 0.5;
  auto a = prev + next - current - current;
  double p = v.x * a.y - v.y * a.x;
  double q = v.x * v.x + v.y * v.y;
  return p / std::sqrt(Cube(q));
}

double Curvature(const Vec2d& prev, const Vec2d& current, const Vec2d& next) {
  return std::abs(CurvatureSigned(prev, current, next));
}

double CurvatureSqr(const Vec2d& prev, const Vec2d& current, const Vec2d& next) {
  auto v = (next - prev) * 0.5;
  auto a = prev + next - current - current;
  double p = v.x * a.y - v.y * a.x;
  double q = v.x * v.x + v.y * v.y;
  return Sqr(p) / Cube(q);
}

double L1Norm(const std::vector<double>& v) {
  double l1_norm = 0;
  for (double x : v) {
    l1_norm += std::abs(x);
  }
  return l1_norm;
}

double L2Norm(const std::vector<double>& v) {
  double l2_norm = 0;
  for (double x : v) {
    l2_norm += Sqr(x);
  }
  return sqrt(l2_norm);
}

double LInfinityNorm(const std::vector<double>& v) {
  double l_infinity_norm = 0;
  for (double x : v) {
    l_infinity_norm = std::max(l_infinity_norm, std::abs(x));
  }
  return l_infinity_norm;
}

double StandardDeviation(const std::vector<double>& v, double average) {
  const double variance = std::accumulate(
      v.begin(), v.end(), 0.0, [average](double a, double b) { return a + Sqr(b - average); });
  return std::sqrt(variance);
}

std::vector<double> PercentilesInPlace(std::vector<double>* v_ptr,
                                       const std::vector<int>& percentages) {
  CHECK(!v_ptr->empty());

  const auto num_data_points = v_ptr->size();
  const auto num_percentages = percentages.size();
  std::vector<double> results;
  results.reserve(num_percentages);

  auto previous_index = 0;
  for (int i = 0; i < num_percentages; ++i) {
    CHECK_GE(percentages[i], (i > 0 ? percentages[i - 1] : 0));
    CHECK_LE(percentages[i], 100);

    const auto current_index = PercentileIndex(num_data_points, percentages[i] / 100.0);
    std::nth_element(v_ptr->begin() + previous_index, v_ptr->begin() + current_index, v_ptr->end());
    results.push_back((*v_ptr)[current_index]);
    previous_index = current_index;
  }

  return results;
}

}  // namespace math

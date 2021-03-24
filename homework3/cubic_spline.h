// Copyright @2020 Pony AI Inc. All rights reserved.

#pragma once

#include <vector>

class CubicSpline {
 public:
  CubicSpline() = default;

  // Set input observed points. Require xs to have strictly increasing order.
  void SetPoints(const std::vector<double>& xs, const std::vector<double>& ys);

  void Interpolate();

  double GetInterpolatedY(double x) const;
};

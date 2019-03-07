// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/color.h"

namespace utils {
namespace display {

interface::display::Color Color::ToProtoBuffer() const {
  interface::display::Color color;
  color.set_red(red);
  color.set_green(green);
  color.set_blue(blue);
  color.set_alpha(alpha);
  return color;
}

Eigen::Vector3d HsvToRgb(Eigen::Vector3d in) {
  if (in.coeff(1) <= 0.0) {  // < is bogus, just shuts up warnings
    return Eigen::Vector3d(in(2), in(2), in(2));
  }

  // See https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV for algorithm.
  double hh = in(0) * 6.0;
  if (hh >= 6.0) {
    hh = 0.0;
  }

  double c = in(1) * in(2);
  double x = c * (1 - std::abs(std::fmod(hh, 2) - 1));

  switch (static_cast<int64_t>(hh)) {
    case 0:
      return Eigen::Vector3d(c, x, 0);
    case 1:
      return Eigen::Vector3d(x, c, 0);
    case 2:
      return Eigen::Vector3d(0, c, x);
    case 3:
      return Eigen::Vector3d(0, x, c);
    case 4:
      return Eigen::Vector3d(x, 0, c);
    case 5:
    default:
      return Eigen::Vector3d(c, 0, x);
  }
}

}  // namespace display
}  // namespace utils

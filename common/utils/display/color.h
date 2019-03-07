// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <Eigen/Eigen>

#include "common/proto/display.pb.h"
#include "common/utils/math/dimension.h"

namespace utils {
namespace display {

struct Color {
  // This is a glue to get the internal type.
  using ElemType = float;
  static constexpr int kValue = 4;

  Color() = default;
  Color(float red, float green, float blue) : red(red), green(green), blue(blue) {}

  Color(float red, float green, float blue, float alpha)
      : red(red), green(green), blue(blue), alpha(alpha) {}

  float red = 0.0f;
  float green = 0.0f;
  float blue = 0.0f;
  float alpha = 1.0f;

  static Color White() { return Color(1.0, 1.0, 1.0); }
  static Color LightGrey() { return Color(0.8, 0.8, 0.8); }
  static Color Grey() { return Color(0.5, 0.5, 0.5); }
  static Color Black() { return Color(0.0, 0.0, 0.0); }
  static Color Blue() { return Color(0.0, 0.0, 1.0); }
  static Color Red() { return Color(1.0, 0.0, 0.0); }
  static Color Green() { return Color(0.0, 1.0, 0.0); }
  static Color Yellow() { return Color(1.0, 1.0, 0.0); }
  static Color LightBlue() { return Color(0.0, 1.0, 1.0); }
  static Color Purple() { return Color(1.0, 0.0, 1.0); }
  static Color Orange() { return Color(1.0, 0.647, 0.0); }
  static Color DarkOrange() { return Color(0.5, 0.3, 0.0); }
  static Color Pink() { return Color(1.0, 0.412, 0.725); }
  static Color VariableRed() { return Color(0.8, 0.0, 0.0); }
  static Color VariableGreen() { return Color(0.0, 0.8, 0.0); }
  static Color VariableYellow() { return Color(0.8, 0.8, 0.0); }

  interface::display::Color ToProtoBuffer() const;
};

Eigen::Vector3d HsvToRgb(Eigen::Vector3d in);

}  // namespace display
}  // namespace utils

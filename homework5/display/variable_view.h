// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>

#include "common/proto/display.pb.h"
#include "common/proto/transform.pb.h"
#include "common/utils/display/color.h"

namespace utils {
namespace display {

class VariableView {
 public:
  // The argument is the variable view name
  explicit VariableView(const std::string& name);

  const interface::display::VariableView& ToProtoBuffer() const;

  void AddVariable(const std::string& variable_name, const std::string& variable_value,
                   Color foreground_color = Color::Black(),
                   Color background_color = Color::White());

  void AddDoubleVariable(const std::string& variable_name, double value);

  void AddDoubleVariable(const std::string& variable_name, double value, int precision);

  void AddIntVariable(const std::string& variable_name, int value);

  void AddVec3dVariable(const std::string& variable_name,
                        const interface::geometry::Vector3d& value, int precision);

  void AddQuaterniondVariable(const std::string& variable_name,
                              const interface::geometry::Quaterniond& quaternion, int precision);

 private:
  interface::display::VariableView variable_view_;
};

}  // namespace display
}  // namespace utils

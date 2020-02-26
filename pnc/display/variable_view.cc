// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/variable_view.h"

#include <iomanip>
#include <sstream>
#include <string>

#include "common/utils/math/transform/transform.h"

namespace utils {
namespace display {

VariableView::VariableView(const std::string& name) { variable_view_.set_name(name); }

const interface::display::VariableView& VariableView::ToProtoBuffer() const {
  return variable_view_;
}

void VariableView::AddVariable(const std::string& variable_name, const std::string& variable_value,
                               Color foreground_color, Color background_color) {
  auto* variable = variable_view_.add_variable();
  variable->set_name(variable_name);
  variable->set_value(variable_value);
  *variable->mutable_foreground_color() = foreground_color.ToProtoBuffer();
  *variable->mutable_background_color() = background_color.ToProtoBuffer();
}

void VariableView::AddDoubleVariable(const std::string& variable_name, double value) {
  AddVariable(variable_name, std::to_string(value));
}

void VariableView::AddIntVariable(const std::string& variable_name, int value) {
  AddVariable(variable_name, std::to_string(value));
}

void VariableView::AddDoubleVariable(const std::string& variable_name, double value,
                                     int precision) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(precision) << value;
  ss.flush();
  AddVariable(variable_name, ss.str());
}

void VariableView::AddVec3dVariable(const std::string& variable_name,
                                    const interface::geometry::Vector3d& value, int precision) {
  AddDoubleVariable(variable_name + ".x", value.x(), precision);
  AddDoubleVariable(variable_name + ".y", value.y(), precision);
  AddDoubleVariable(variable_name + ".z", value.z(), precision);
}

void VariableView::AddQuaterniondVariable(const std::string& variable_name,
                                          const interface::geometry::Quaterniond& quaternion,
                                          int precision) {
  AddDoubleVariable(variable_name + ".yaw", math::transform::GetYaw(quaternion), precision);
}

void VariableView::Clear() { variable_view_.clear_variable(); }

}  // namespace display
}  // namespace utils

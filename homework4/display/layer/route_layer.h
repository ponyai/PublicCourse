// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "common/proto/route.pb.h"
#include "common/utils/display/layer.h"
#include "homework4/display/map_gl_painter.h"
#include "homework4/display/user_interface_data.h"

namespace utils {
namespace display {

class RouteLayer : public utils::display::Layer {
 public:
  explicit RouteLayer(const std::string& name, const interface::route::Route& data,
                      const UserInterfaceData& user_interface_data)
      : Layer(name), data_(data), user_interface_data_(user_interface_data) {}
  ~RouteLayer() override = default;

  void Draw() const override;

 private:
  const interface::route::Route& data_;
  const UserInterfaceData& user_interface_data_;
};

}  // namespace display
}  // namespace utils

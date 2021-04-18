// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "common/proto/map_lane.pb.h"
#include "common/utils/display/layer.h"
#include "homework4/display/map_gl_painter.h"

namespace utils {
namespace display {

class LaneLayer : public utils::display::Layer {
 public:
  explicit LaneLayer(const std::string& name, const std::vector<interface::map::Lane>& data)
      : Layer(name), data_(data) {}
  ~LaneLayer() override = default;

  void Draw() const override;

 private:
  const std::vector<interface::map::Lane>& data_;
};

}  // namespace display
}  // namespace utils

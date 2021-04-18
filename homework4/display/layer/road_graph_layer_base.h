// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/map.pb.h"
#include "common/utils/display/layer.h"
#include "homework4/map/map_lib.h"

namespace utils {
namespace display {

class RoadGraphLayerBase : public Layer {
 public:
  RoadGraphLayerBase(const std::string& name, const homework4::map::MapLib* map_lib) : Layer(name) {
    map_data_.CopyFrom(map_lib->map_proto());
  }
  ~RoadGraphLayerBase() override = default;

 protected:
  const interface::map::Map& map_data() const { return map_data_; }

 private:
  interface::map::Map map_data_;

  DISALLOW_COPY_MOVE_AND_ASSIGN(RoadGraphLayerBase);
};

}  // namespace display
}  // namespace utils

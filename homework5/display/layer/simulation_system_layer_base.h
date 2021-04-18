// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/simulation.pb.h"
#include "common/utils/display/layer.h"

namespace utils {
namespace display {

class SimulationSystemLayerBase : public Layer {
 public:
  SimulationSystemLayerBase(const std::string& name,
                            const interface::simulation::SimulationSystemData& data)
      : Layer(name), data_(data) {}
  ~SimulationSystemLayerBase() override = default;

 protected:
  const interface::simulation::SimulationSystemData& data_;
};

}  // namespace display
}  // namespace utils

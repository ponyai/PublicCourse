// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>
#include <string>
#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"
#include "common/proto/display.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "pnc/display/variable_view.h"
#include "pnc/map/map_lib.h"

namespace simulation {

class VehicleAgent {
 public:
  explicit VehicleAgent(const std::string& name) : name_(name) {
    CHECK(file::ReadFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
    debug_variables_ = std::make_unique<utils::display::VariableView>(name);
  }
  virtual ~VehicleAgent() = default;

  // Vehicle's parameters
  const interface::vehicle::VehicleParams& vehicle_params() const { return vehicle_params_; }
  // Map lib to get the map
  const pnc::map::MapLib& map_lib() const { return map_lib_; }
  // Name of the vehicle
  const std::string& name() const { return name_; }

  // Clean all debug variables, will be called before RunOneIteration call
  void clear_debug_variables() { debug_variables_->Clear(); }
  // Return
  const interface::display::VariableView& debug_variables() {
    return debug_variables_->ToProtoBuffer();
  }

  // Initialize the agent of with the initial agent status
  virtual void Initialize(const interface::agent::AgentStatus& agent_status) = 0;
  // Given the current agent status, and return the control command in this iteration
  virtual interface::control::ControlCommand RunOneIteration(
      const interface::agent::AgentStatus& agent_status) = 0;

 protected:
  void PublishVariable(const std::string& key, const std::string& value,
                       utils::display::Color foreground_color = utils::display::Color::Black(),
                       utils::display::Color background_color = utils::display::Color::White()) {
    debug_variables_->AddVariable(key, value, foreground_color, background_color);
  }

 private:
  interface::vehicle::VehicleParams vehicle_params_;
  pnc::map::MapLib map_lib_;
  std::string name_;
  std::unique_ptr<utils::display::VariableView> debug_variables_;
};

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>
#include <string>
#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "homework6/map/map_lib.h"

namespace simulation {

// Base class of vehicle agent
class VehicleAgent {
 public:
  explicit VehicleAgent(const std::string& name) : name_(name) {
    CHECK(file::ReadFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_))
        << "Can not open vehicle parameter file from the specific path";
  }
  virtual ~VehicleAgent() = default;

  // Vehicle's parameters
  const interface::vehicle::VehicleParams& vehicle_params() const { return vehicle_params_; }
  // Map lib of the lib
  const homework6::map::MapLib& map_lib() const { return map_lib_; }
  // Name of the agent
  const std::string& name() const { return name_; }

  // Initialize the agent of with the initial agent status
  virtual void Initialize(const interface::agent::AgentStatus& agent_status) = 0;
  // Given the current agent status, and return the control command in this iteration
  virtual interface::control::ControlCommand RunOneIteration(
      const interface::agent::AgentStatus& agent_status) = 0;

 private:
  interface::vehicle::VehicleParams vehicle_params_;
  homework6::map::MapLib map_lib_;
  std::string name_;
};

}  // namespace simulation

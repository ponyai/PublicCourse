// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <vector>

#include "common/proto/agent_status.pb.h"
#include "common/proto/map.pb.h"

#include "Eigen/Core"
#include "Eigen/Dense"

#include "common/utils/math/transform/transform.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"
#include "homework6/proto/simulation_config.pb.h"
#include "homework6/simulation/dynamic_lib/lib_vehicle_status_model_solver.h"

namespace simulation {

class SimulationWorld {
 public:
  SimulationWorld(const interface::map::Map& map,
                  const interface::homework6::SimulationConfig& simulation_config,
                  const interface::vehicle::VehicleParams& vehicle_params) {
    map_.CopyFrom(map);
    vehicle_params_.CopyFrom(vehicle_params);
    simulation_config_.CopyFrom(simulation_config);
  }
  virtual ~SimulationWorld() = default;

  void Initialize();

  void RunOneIteration();
  bool DetermineSimulationFinished();

  void UpdateControlCommand(const std::unordered_map<
                            std::string, interface::control::ControlCommand>& control_command_map) {
    control_command_map_.clear();
    control_command_map_ = control_command_map;
  }

  const std::unordered_map<std::string, interface::agent::AgentStatus>& GetAgentStatusMap() {
    return agent_status_map_;
  }

  double GetSimulationTime() { return current_time_; }

 private:
  bool DetermineReachedDestination(const interface::agent::VehicleStatus& vehicle_status,
                                   const interface::agent::RouteStatus& route_status);
  void AddVehicleAgent(const std::string& name);
  void UpdateSimulationTime();
  void UpdateTripInfo();
  void UpdateVehicleStatus();
  interface::agent::AgentStatus GenerateInitialStatusFromRoute();

  interface::map::Map map_;

  double current_time_ = 0.0;

  std::unordered_map<std::string, std::unique_ptr<vehicle_status_model::VehicleStatusModelSolver>>
      dynamic_solver_map_;
  std::unordered_map<std::string, interface::control::ControlCommand> control_command_map_;
  std::unordered_map<std::string, interface::agent::AgentStatus> agent_status_map_;

  interface::vehicle::VehicleParams vehicle_params_;

  interface::homework6::SimulationConfig simulation_config_;
};

};  // namespace simulation

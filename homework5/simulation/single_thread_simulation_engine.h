// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <vector>

#include "homework6/simulation/simulation_engine_base.h"
#include "homework6/simulation/vehicle_agent.h"
#include "homework6/simulation/vehicle_agent_factory.h"

namespace simulation {

class SingleThreadSimulationEngine : public SimulationEngineBase {
 public:
  explicit SingleThreadSimulationEngine(
      const interface::homework6::SimulationConfig& simulation_config)
      : SimulationEngineBase(simulation_config) {}
  ~SingleThreadSimulationEngine() override = default;

  void InitializeInternal() override {
    VehicleAgent* vehicle_agent = VehicleAgentFactory::Instance()->Create(
        simulation_config_.agent_type(), simulation_config_.agent_name());
    cars_.push_back(std::unique_ptr<VehicleAgent>(vehicle_agent));
    vehicle_agent->Initialize(agent_status_map_[simulation_config_.agent_name()]);
  }

  void RunOneIterationInternal() override {
    control_command_map_.clear();
    for (auto& iter : cars_) {
      VehicleAgent& vehicle_agent = *iter;
      const std::string& name = vehicle_agent.name();
      control_command_map_[name].CopyFrom(vehicle_agent.RunOneIteration(agent_status_map_[name]));
    }
  }

 private:
  std::vector<std::unique_ptr<VehicleAgent>> cars_;
};
};

// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/single_thread_simulation_engine.h"

namespace simulation {

using interface::simulation::AgentConfig;

void SingleThreadSimulationEngine::InitializeInternal() {
  LOG(INFO) << "Single threaded simulation engine initialization started.";
  for (const AgentConfig& agent_config : simulation_config_.agent_config()) {
    VehicleAgent* vehicle_agent =
        VehicleAgentFactory::Instance()->Create(agent_config.type(), agent_config.name());
    cars_.push_back(std::unique_ptr<VehicleAgent>(vehicle_agent));
    vehicle_agent->Initialize(agent_status_map_[agent_config.name()]);
  }
  LOG(INFO) << "Single threaded simulation engine initialization finished.";
}

void SingleThreadSimulationEngine::RunOneIterationInternal() {
  agent_response_map_.clear();
  for (auto& iter : cars_) {
    VehicleAgent& vehicle_agent = *iter;
    const std::string& name = vehicle_agent.name();
    if (!agent_status_map_[name].simulation_status().is_alive()) {
      continue;
    }
    vehicle_agent.clear_debug_variables();
    auto start = std::chrono::system_clock::now();
    interface::control::ControlCommand control_command =
        vehicle_agent.RunOneIteration(agent_status_map_[name]);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    agent_response_map_[name].mutable_control_command()->Swap(&control_command);
    agent_response_map_[name].set_iteration_time_cost(diff.count());
    agent_response_map_[name].mutable_debug_variables()->CopyFrom(vehicle_agent.debug_variables());
  }
}

}  // namespace simulation

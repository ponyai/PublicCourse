// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/simulation/simulation_system.h"
#include "common/proto/agent_status.pb.h"
#include "common/proto/simulation.pb.h"
#include "common/utils/common/mutex.h"

#include <glog/logging.h>
#include "homework6/map/map_lib.h"

namespace simulation {

SimulationSystem::SimulationSystem(const interface::homework6::SimulationConfig& simulation_config)
    : simulation_config_(simulation_config) {}

void SimulationSystem::Initialize() {
  map_lib_ = std::make_unique<homework6::map::MapLib>();
  CHECK(file::ReadTextFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));

  simulation_world_ =
      std::make_unique<SimulationWorld>(map_lib_->map_proto(), simulation_config_, vehicle_params_);
  simulation_world_->Initialize();

  simulation_engine_ = std::make_unique<SingleThreadSimulationEngine>(simulation_config_);
  simulation_engine_->SetAgentStatusMap(simulation_world_->GetAgentStatusMap());
  simulation_engine_->Initialize();
}

void SimulationSystem::Start() {
  LOG(INFO) << "Simulation started";
  std::unordered_map<std::string, interface::control::ControlCommand> control_command_map;
  auto system_time = std::chrono::steady_clock::now();
  while (true) {
    FlushPlaybackCommands();
    if (!playback_status().playback_paused || MaybePlaybackNextOneIteration()) {
      {
        utils::MutexLocker lock(&mutex_);
        agent_status_map_ = simulation_world_->GetAgentStatusMap();
        simulation_time_ = simulation_world_->GetSimulationTime();
      }
      simulation_engine_->SetAgentStatusMap(simulation_world_->GetAgentStatusMap());
      simulation_engine_->RunOneIteration();
      control_command_map_ = simulation_engine_->GetControlCommandMap();
      simulation_world_->UpdateControlCommand(control_command_map_);
      simulation_world_->RunOneIteration();
      if (simulation_world_->DetermineSimulationFinished()) {
        break;
      }
    }
    system_time +=
        std::chrono::microseconds(static_cast<int64_t>(10000 / playback_status().playback_speed_x));
    std::this_thread::sleep_until(system_time);
  }
  {
    utils::MutexLocker lock(&mutex_);
    agent_status_map_ = simulation_world_->GetAgentStatusMap();
    simulation_time_ = simulation_world_->GetSimulationTime();
  }
}

interface::simulation::SimulationSystemData SimulationSystem::FetchData() {
  interface::simulation::SimulationSystemData simulation_system_data;
  {
    utils::MutexLocker lock(&mutex_);
    for (auto& iter : agent_status_map_) {
      interface::simulation::VehicleAgentData* vehicle_agent_data =
          simulation_system_data.add_vehicle_agent();
      vehicle_agent_data->set_name(iter.first);
      vehicle_agent_data->mutable_agent_status()->CopyFrom(iter.second);
    }
    simulation_system_data.set_simulation_time(simulation_time_);
  }
  return simulation_system_data;
}

}  // namespace simulation

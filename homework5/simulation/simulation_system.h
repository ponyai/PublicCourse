// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <chrono>
#include <queue>
#include <thread>
#include <vector>

#include "common/proto/agent_status.pb.h"
#include "common/proto/simulation.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/common/mutex.h"
#include "gflags/gflags.h"
#include "homework6/map/map_lib.h"
#include "homework6/proto/simulation_config.pb.h"
#include "homework6/simulation/simulation_engine_base.h"
#include "homework6/simulation/simulation_world.h"
#include "homework6/simulation/single_thread_simulation_engine.h"
#include "homework6/simulation/system_base.h"
#include "homework6/simulation/vehicle_agent.h"

namespace simulation {

class SimulationSystem : public SystemBase {
 public:
  explicit SimulationSystem(const interface::homework6::SimulationConfig& simulation_config);
  ~SimulationSystem() override = default;

  virtual interface::simulation::SimulationSystemData FetchData() override;

  void Initialize();
  void Start();

 private:
  utils::Mutex mutex_{"SimulationSystemData"};

  std::unordered_map<std::string, interface::agent::AgentStatus> agent_status_map_
      GUARDED_BY(mutex_);
  double simulation_time_ GUARDED_BY(mutex_);

  std::unique_ptr<homework6::map::MapLib> map_lib_;

  std::unordered_map<std::string, interface::control::ControlCommand> control_command_map_;

  std::unique_ptr<SimulationEngineBase> simulation_engine_;
  std::unique_ptr<SimulationWorld> simulation_world_;

  interface::homework6::SimulationConfig simulation_config_;
  interface::vehicle::VehicleParams vehicle_params_;
};

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <chrono>
#include <queue>
#include <thread>
#include <vector>

#include "common/proto/agent_metric.pb.h"
#include "common/proto/agent_status.pb.h"
#include "common/proto/simulation.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/common/mutex.h"
#include "gflags/gflags.h"
#include "pnc/map/map_lib.h"
#include "pnc/simulation/manual_test_agent.h"
#include "pnc/simulation/metric_collector.h"
#include "pnc/simulation/multi_process/multi_process_simulation_engine.h"
#include "pnc/simulation/simulation_engine_base.h"
#include "pnc/simulation/simulation_world.h"
#include "pnc/simulation/single_thread_simulation_engine.h"
#include "pnc/simulation/system_base.h"
#include "pnc/simulation/types.h"
#include "pnc/simulation/vehicle_agent.h"

DECLARE_bool(write_simulation_history);
DECLARE_string(simulation_log_file_path);
DECLARE_string(simulation_metrics_file_path);
DECLARE_bool(multi_prcess_mode);
DECLARE_bool(enable_visualization);
DECLARE_bool(enable_debug_variables);

namespace simulation {

class SimulationSystem : public SystemBase {
 public:
  explicit SimulationSystem(const interface::simulation::SimulationConfig& simulation_config);
  ~SimulationSystem() override = default;

  virtual interface::simulation::SimulationSystemData FetchData() override;

  void Initialize();
  void Start();

  ManualTestAgent* manual_test_agent() { return manual_test_agent_.get(); }

 private:
  VehicleAgent* AddManualTestAgent();

  void LogSimulationMetric();
  void LogSimulationHistory();

  utils::Mutex mutex_{"SimulationSystemData"};

  double simulation_time_ GUARDED_BY(mutex_);
  interface::perception::PerceptionTrafficLightStatus traffic_light_status_ GUARDED_BY(mutex_);
  AgentStatusMap agent_status_map_ GUARDED_BY(mutex_);
  std::vector<interface::perception::PerceptionObstacle> pedestrian_obstacles_ GUARDED_BY(mutex_);
  std::unordered_map<std::string, interface::display::VariableView> debug_variables_map_map_
      GUARDED_BY(mutex_);

  AgentResponseMap agent_response_map_;
  std::unique_ptr<pnc::map::MapLib> map_lib_;
  std::unique_ptr<SimulationWorld> simulation_world_;
  std::unique_ptr<ManualTestAgent> manual_test_agent_;
  std::unique_ptr<file::FileInterface> simulation_log_file_;
  std::unique_ptr<file::FileInterface> simulation_index_file_;
  std::unique_ptr<SimulationEngineBase> simulation_engine_;

  interface::simulation::SimulationConfig simulation_config_;
  interface::vehicle::VehicleParams vehicle_params_;
};

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/map.pb.h"
#include "common/proto/simulation.pb.h"
#include "common/proto/simulation_config.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "pnc/map/segment.h"
#include "pnc/simulation/types.h"

#include "common/utils/index/grid_index.h"
#include "common/utils/math/box2d.h"
#include "pnc/map/map_meta.h"
#include "pnc/simulation/metric_collector.h"
#include "pnc/utils/vehicle.h"

namespace simulation {

class SimulationEvaluator {
 public:
  explicit SimulationEvaluator(const interface::simulation::SimulationConfig& simulation_config,
                               pnc::map::MapMeta* map_meta)
      : simulation_config_(simulation_config), map_meta_(map_meta) {
    CHECK(file::ReadTextFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
  }

  void Initialize(const AgentStatusMap& agent_status_map);

  void EvaluateOneIteration(const AgentResponseMap& agent_response_map,
                            const std::vector<math::Box2d>& pedestrian_boxes,
                            AgentStatusMap* agent_status_map);

  void UpdateTrafficLightStatus(
      const interface::perception::PerceptionTrafficLightStatus& traffic_light_status);

  interface::simulation::SimulationMetrics GetSimulationMetric();

 private:
  void EvaluateVehicleExceedSpeedLimit(AgentStatusMap* agent_status_map);
  void EvaluateCollision(const std::vector<math::Box2d>& pedestrian_boxes,
                         AgentStatusMap* agent_status_map);
  void EvaluateOutOfLane(AgentStatusMap* agent_status_map);
  void EvaluateRedLightRunning(AgentStatusMap* agent_status_map);
  void EvaluateIterationTimeout(const AgentResponseMap& agent_response_map,
                                AgentStatusMap* agent_status_map);

  bool InCollision(const interface::agent::VehicleStatus& vehicle_status1,
                   const interface::agent::VehicleStatus& vehicle_status2);
  bool RearEnd(const interface::agent::VehicleStatus& vehicle_status1,
               const interface::agent::VehicleStatus& vehicle_status2);
  bool InCollision(const math::Box2d& box1, const interface::agent::VehicleStatus& vehicle_status);

  interface::simulation::SimulationConfig simulation_config_;
  interface::vehicle::VehicleParams vehicle_params_;

  pnc::map::MapMeta* map_meta_;  // Not owned
  interface::perception::PerceptionTrafficLightStatus traffic_light_status_;
  std::unordered_map<std::string, interface::map::Bulb::Color> traffic_light_color_map_;
  std::unordered_map<std::string, std::string> vehicle_in_front_of_stop_line_map_;
  std::unordered_map<std::string, std::unique_ptr<MetricCollector>> metric_collector_map_;
  std::unordered_map<std::string, int> soft_limit_hit_map_;
  std::unordered_map<std::string, double> last_moving_time_map_;
};
};

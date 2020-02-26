// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <vector>

#include "common/proto/agent_status.pb.h"
#include "common/proto/map.pb.h"
#include "common/proto/simulation_config.pb.h"

#include "Eigen/Core"
#include "Eigen/Dense"

#include "common/utils/index/grid_index.h"
#include "common/utils/math/box2d.h"
#include "common/utils/math/transform/transform.h"
#include "pnc/map/map_meta.h"
#include "pnc/map/segment.h"
#include "pnc/simulation/dynamic_lib/lib_vehicle_status_model_solver.h"
#include "pnc/simulation/mocked_obstacle_agent.h"
#include "pnc/simulation/simulation_evaluator.h"
#include "pnc/simulation/traffic_light_agent.h"
#include "pnc/utils/vehicle.h"
#include "pnc/utils/vehicle_status_helper.h"

namespace simulation {

class SimulationWorld {
 public:
  SimulationWorld(const interface::map::Map& map,
                  const interface::vehicle::VehicleParams& vehicle_params,
                  const interface::simulation::SimulationConfig& simulation_config) {
    map_.CopyFrom(map);
    vehicle_params_.CopyFrom(vehicle_params);
    simulation_config_.CopyFrom(simulation_config);
    GenerateInitialStatusCandidates();
  }
  virtual ~SimulationWorld() = default;

  void Initialize() {
    map_meta_ = std::make_unique<pnc::map::MapMeta>(map_);
    map_meta_->Initialize();
    traffic_light_agent_ = std::make_unique<TrafficLightAgent>(map_);
    mocked_obstacle_agent_ =
        std::make_unique<MockedObstacleAgent>(simulation_config_, map_meta_.get());
    mocked_obstacle_agent_->Initialize();
    simulation_evaluator_ =
        std::make_unique<SimulationEvaluator>(simulation_config_, map_meta_.get());
  }
  // All initialization has been finished, start the world
  void Start();

  interface::agent::AgentStatus AddVehicleAgent(const std::string& name);

  void RunOneIteration();
  bool DeterminSimulationFinished();
  void EvaluateAlive();

  void UpdateAgentResponse(const AgentResponseMap& agent_response_map) {
    agent_response_map_ = agent_response_map;
  }

  const AgentStatusMap& GetAgentStatusMap() { return agent_status_map_; }

  const interface::perception::PerceptionTrafficLightStatus& GetTrafficLightStatus() {
    return traffic_light_agent_->GetTrafficLightStatus();
  }

  const std::vector<interface::perception::PerceptionObstacle>& GetPedestrianObstacles() {
    return pedestrian_obstacles_;
  }

  double GetSimulationTime() { return current_time_; }

  interface::simulation::SimulationMetrics GetSimulationMetric() {
    return simulation_evaluator_->GetSimulationMetric();
  }

 private:
  void GenerateInitialStatusCandidates();

  interface::agent::VehicleStatus GenerateRandomInitialStatus();
  interface::geometry::Point3D GenerateRandomRouterRequest();

  bool DetermineReachedDestination(const interface::agent::VehicleStatus& vehicle_status,
                                   const interface::agent::RouteStatus& route_status);
  void UpdateSimulationTime();
  void StartNewTrip(const std::string& name);
  void UpdateTripInfo();
  void UpdateVehicleStatus();
  void UpdatePerceptionStatus();

  double current_time_ = 0.0;

  interface::map::Map map_;
  std::unique_ptr<pnc::map::MapMeta> map_meta_;

  std::unordered_map<std::string, std::unique_ptr<vehicle_status_model::VehicleStatusModelSolver>>
      dynamic_solver_map_;
  AgentResponseMap agent_response_map_;
  AgentStatusMap agent_status_map_;

  std::unique_ptr<TrafficLightAgent> traffic_light_agent_;
  interface::perception::PerceptionTrafficLightStatus traffic_light_status_;

  std::unique_ptr<MockedObstacleAgent> mocked_obstacle_agent_;
  std::vector<math::Box2d> pedestrian_boxes_;
  std::vector<interface::perception::PerceptionObstacle> pedestrian_obstacles_;

  interface::vehicle::VehicleParams vehicle_params_;

  std::unique_ptr<SimulationEvaluator> simulation_evaluator_;
  interface::simulation::SimulationConfig simulation_config_;

  std::vector<interface::agent::VehicleStatus> initial_vehicle_status_candidates_;
  int current_initial_vehicle_status_index_ = 0;
};

};  // namespace simulation

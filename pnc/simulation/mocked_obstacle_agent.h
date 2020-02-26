// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/perception.pb.h"
#include "common/proto/simulation_config.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/file/path.h"
#include "common/utils/math/box2d.h"
#include "pnc/map/map_meta.h"
#include "pnc/simulation/types.h"
#include "pnc/utils/vehicle.h"

constexpr int kNumPedestrianAgents = 30;
constexpr double kIterationTime = 0.01;
constexpr double kPedestrianHeight = 1.8;

namespace simulation {

class MockedObstacleAgent {
 public:
  MockedObstacleAgent(interface::simulation::SimulationConfig simulation_config,
                      pnc::map::MapMeta* map_meta) {
    simulation_config_.CopyFrom(simulation_config);
    map_meta_ = map_meta;
    CHECK(file::ReadTextFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
    min_x_ = map_meta_->min_x() - 10.0;
    min_y_ = map_meta_->min_y() - 10.0;
    max_x_ = map_meta_->max_x() + 10.0;
    max_y_ = map_meta_->max_y() + 10.0;
  }

  std::vector<math::Box2d> RunOneIteration(const AgentStatusMap& agent_status_map);
  void Initialize();

 private:
  math::Vec2d GenerateRandomCenter();

  interface::simulation::SimulationConfig simulation_config_;
  pnc::map::MapMeta* map_meta_;  // Not owned

  std::vector<math::Vec2d> centers_;
  std::vector<double> headings_;
  std::vector<double> speeds_;
  std::vector<int> stop_moving_count_;

  double min_x_;
  double min_y_;
  double max_x_;
  double max_y_;

  interface::vehicle::VehicleParams vehicle_params_;
};

};  // namespace simulation

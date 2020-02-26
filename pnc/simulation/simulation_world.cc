// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/simulation_world.h"
#include "common/proto/perception.pb.h"
#include "pnc/utils/vehicle.h"

#include "glog/logging.h"
#include "gflags/gflags.h"

using interface::agent::AgentStatus;
using interface::agent::VehicleStatus;
using interface::agent::SimulationStatus;
using interface::agent::PerceptionStatus;
using interface::perception::PerceptionObstacle;

namespace {

constexpr double kMaximumPerceptionObstacleDistance = 100.0;

double DistanceToObstacle(const VehicleStatus& vehicle_status, const PerceptionObstacle& obstacle) {
  math::Vec2d vehicle_point(vehicle_status.position().x(), vehicle_status.position().y());
  // Assume the obstacle is not large and it is far away from the vehicle, then pick any one point
  // from obstacle,
  math::Vec2d obstacle_point(obstacle.polygon_point(0).x(), obstacle.polygon_point(0).y());
  return vehicle_point.DistanceToPoint(obstacle_point);
}
}

namespace simulation {

void SimulationWorld::GenerateInitialStatusCandidates() {
  for (const interface::map::Lane& lane : map_.lane()) {
    const interface::geometry::Polyline& left_bound = lane.left_bound().boundary();
    if (left_bound.point_size() == 2) {
      const interface::geometry::Polyline& central_line = lane.central_line();
      if (central_line.point_size() > 80) {
        int index = std::rand() % (central_line.point_size() - 40) + 20;
        double current_x = central_line.point(index).x();
        double current_y = central_line.point(index).y();
        double next_x = central_line.point(index + 1).x();
        double next_y = central_line.point(index + 1).y();
        double yaw = atan2(next_y - current_y, next_x - current_x);
        Eigen::Quaterniond quat = math::transform::RollPitchYaw(0, 0, yaw);
        interface::agent::VehicleStatus vehicle_status;
        vehicle_status.mutable_position()->set_x(central_line.point(index).x());
        vehicle_status.mutable_position()->set_y(central_line.point(index).y());
        vehicle_status.mutable_position()->set_z(central_line.point(index).z());
        vehicle_status.mutable_orientation()->set_x(quat.x());
        vehicle_status.mutable_orientation()->set_y(quat.y());
        vehicle_status.mutable_orientation()->set_z(quat.z());
        vehicle_status.mutable_orientation()->set_w(quat.w());
        initial_vehicle_status_candidates_.push_back(vehicle_status);
      }
    }
  }
  current_initial_vehicle_status_index_ = 0;
}

VehicleStatus SimulationWorld::GenerateRandomInitialStatus() {
  CHECK(current_initial_vehicle_status_index_ < initial_vehicle_status_candidates_.size())
      << "Generated too many vehicle agents";
  VehicleStatus result = initial_vehicle_status_candidates_[current_initial_vehicle_status_index_];
  current_initial_vehicle_status_index_++;
  return result;
}

interface::geometry::Point3D SimulationWorld::GenerateRandomRouterRequest() {
  std::vector<interface::geometry::Point3D> candidates;
  for (const interface::map::Lane& lane : map_.lane()) {
    interface::geometry::Point3D point;
    const interface::geometry::Polyline& left_bound = lane.left_bound().boundary();
    if (left_bound.point_size() == 2) {
      const interface::geometry::Polyline& central_line = lane.central_line();
      if (central_line.point_size() > 80) {
        int index = std::rand() % (central_line.point_size() - 40) + 20;
        point.set_x(central_line.point(index).x());
        point.set_y(central_line.point(index).y());
        candidates.push_back(point);
      }
    }
  }
  return candidates[std::rand() % candidates.size()];
}

interface::agent::AgentStatus SimulationWorld::AddVehicleAgent(const std::string& name) {
  interface::agent::AgentStatus agent_status;
  agent_status.mutable_vehicle_status()->CopyFrom(GenerateRandomInitialStatus());
  std::unique_ptr<vehicle_status_model::VehicleStatusModelSolver> solver =
      vehicle_status_model::CreateVehicleStatusModelSolver(vehicle_params_);
  solver->Initialize(0, agent_status.vehicle_status());
  dynamic_solver_map_[name] = std::move(solver);

  interface::agent::SimulationStatus simulation_status;
  simulation_status.set_is_alive(true);
  agent_status.mutable_simulation_status()->CopyFrom(simulation_status);

  agent_status_map_[name].CopyFrom(agent_status);
  return agent_status;
}

void SimulationWorld::StartNewTrip(const std::string& name) {
  interface::agent::AgentStatus& agent_status = agent_status_map_.at(name);
  CHECK(!agent_status.has_route_status());
  interface::agent::RouteStatus route_status;
  interface::geometry::Point3D point = GenerateRandomRouterRequest();
  route_status.mutable_destination()->CopyFrom(point);
  route_status.set_is_new_request(true);
  agent_status.mutable_route_status()->CopyFrom(route_status);
}

void SimulationWorld::Start() {
  for (auto& iter : agent_status_map_) {
    StartNewTrip(iter.first);
  }
  simulation_evaluator_->Initialize(agent_status_map_);
}

void SimulationWorld::UpdateTripInfo() {
  for (auto& iter : agent_status_map_) {
    const std::string& name = iter.first;
    interface::agent::AgentStatus& agent_status = iter.second;
    if (agent_status.simulation_status().is_alive()) {
      if (DetermineReachedDestination(agent_status.vehicle_status(), agent_status.route_status())) {
        agent_status.clear_route_status();
        int num_finished_trips = agent_status.simulation_status().num_finished_trips() + 1;
        agent_status.mutable_simulation_status()->set_num_finished_trips(num_finished_trips);
        if (num_finished_trips == simulation_config_.task_config().num_trips()) {
          agent_status.mutable_simulation_status()->set_is_alive(false);
          agent_status.mutable_simulation_status()->set_is_finished(true);
        } else {
          StartNewTrip(name);
        }
      } else {
        agent_status.mutable_route_status()->set_is_new_request(false);
      }
    }
  }
}

void SimulationWorld::UpdateVehicleStatus() {
  for (auto& iter : agent_status_map_) {
    const interface::agent::AgentStatus& agent_status = iter.second;
    const interface::agent::SimulationStatus& simulation_status = agent_status.simulation_status();
    if (simulation_status.is_alive()) {
      const std::string& name = iter.first;
      if (agent_response_map_.find(name) == agent_response_map_.end()) {
        LOG(ERROR) << "Time:" << current_time_ << ",  simulation world does not receive " << name
                   << "'s control command";
        continue;
      }
      agent_status_map_.at(name).mutable_vehicle_status()->CopyFrom(
          dynamic_solver_map_[name]->UpdateVehicleStatus(
              current_time_, agent_response_map_[name].control_command()));
    }
  }
}

void SimulationWorld::EvaluateAlive() {
  simulation_evaluator_->EvaluateOneIteration(agent_response_map_, pedestrian_boxes_,
                                              &agent_status_map_);
}

void SimulationWorld::UpdateSimulationTime() {
  for (auto& iter : agent_status_map_) {
    iter.second.mutable_simulation_status()->set_simulation_time(current_time_);
  }
}

void SimulationWorld::UpdatePerceptionStatus() {
  std::vector<PerceptionObstacle> obstacles;
  for (auto& iter : agent_status_map_) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    const AgentStatus& agent_status = iter.second;
    const std::string& name = iter.first;
    obstacles.push_back(
        utils::vehicle::ToPerceptionObstacle(agent_status.vehicle_status(), vehicle_params_, name));
  }
  if (simulation_config_.add_mocked_pedestrians()) {
    pedestrian_boxes_ = mocked_obstacle_agent_->RunOneIteration(agent_status_map_);
    pedestrian_obstacles_.clear();
    for (int i = 0; i < pedestrian_boxes_.size(); i++) {
      pedestrian_obstacles_.push_back(utils::vehicle::ToPerceptionObstacle(
          pedestrian_boxes_[i], kPedestrianHeight, "P" + std::to_string(i),
          interface::perception::PEDESTRIAN));
    }
    obstacles.insert(obstacles.end(), pedestrian_obstacles_.begin(), pedestrian_obstacles_.end());
  }
  for (auto& iter : agent_status_map_) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    const std::string& name = iter.first;
    AgentStatus& agent_status = iter.second;
    agent_status.clear_perception_status();
    for (const auto& obstacle : obstacles) {
      if (obstacle.id() != name &&
          DistanceToObstacle(agent_status.vehicle_status(), obstacle) <
              kMaximumPerceptionObstacleDistance) {
        agent_status.mutable_perception_status()->add_obstacle()->CopyFrom(obstacle);
      }
    }
    agent_status.mutable_perception_status()->add_traffic_light()->CopyFrom(traffic_light_status_);
  }
}

void SimulationWorld::RunOneIteration() {
  traffic_light_agent_->SetCurrentTime(current_time_);
  if (traffic_light_agent_->StatusRefreshed()) {
    traffic_light_status_.CopyFrom(traffic_light_agent_->GetTrafficLightStatus());
    simulation_evaluator_->UpdateTrafficLightStatus(traffic_light_status_);
  }
  UpdateVehicleStatus();
  UpdatePerceptionStatus();
  UpdateTripInfo();
  EvaluateAlive();
  current_time_ += 0.01;
  UpdateSimulationTime();
}

bool SimulationWorld::DetermineReachedDestination(
    const interface::agent::VehicleStatus& vehicle_status,
    const interface::agent::RouteStatus& route_status) {
  math::Vec2d pos_in_router_request(route_status.destination().x(), route_status.destination().y());
  math::Vec2d current_pos(vehicle_status.position().x(), vehicle_status.position().y());
  utils::vehicle::VehicleStatusHelper helper(vehicle_status);
  return pos_in_router_request.DistanceToPoint(current_pos) < 2.0 &&
         helper.velocity_vcs().x() < 0.1;
}

bool SimulationWorld::DeterminSimulationFinished() {
  // At most run 30 min in simulation time
  if (current_time_ > 30.0 * 60) {
    return true;
  }
  for (auto& iter : agent_status_map_) {
    const interface::agent::AgentStatus& agent_status = iter.second;
    if (agent_status.simulation_status().is_alive()) {
      return false;
    }
  }
  LOG(INFO) << "All vehicles are not alive. Simulation is finihsed.";
  return true;
}

};  // namespace simulation

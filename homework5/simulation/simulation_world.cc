// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/simulation/simulation_world.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

namespace simulation {

using interface::agent::AgentStatus;
using interface::agent::VehicleStatus;
using interface::agent::SimulationStatus;

interface::agent::AgentStatus SimulationWorld::GenerateInitialStatusFromRoute() {
  interface::agent::AgentStatus agent_status;
  agent_status.mutable_vehicle_status()->mutable_position()->set_x(
      simulation_config_.route().start_point().x());
  agent_status.mutable_vehicle_status()->mutable_position()->set_y(
      simulation_config_.route().start_point().y());
  agent_status.mutable_vehicle_status()->mutable_position()->set_z(0);
  agent_status.mutable_vehicle_status()->mutable_orientation()->set_x(
      simulation_config_.route().start_orientation().x());
  agent_status.mutable_vehicle_status()->mutable_orientation()->set_y(
      simulation_config_.route().start_orientation().y());
  agent_status.mutable_vehicle_status()->mutable_orientation()->set_z(
      simulation_config_.route().start_orientation().z());
  agent_status.mutable_vehicle_status()->mutable_orientation()->set_w(
      simulation_config_.route().start_orientation().w());
  agent_status.mutable_route_status()->mutable_destination()->set_x(
      simulation_config_.route().end_point().x());
  agent_status.mutable_route_status()->mutable_destination()->set_y(
      simulation_config_.route().end_point().y());
  agent_status.mutable_route_status()->mutable_destination()->set_z(0);
  agent_status.mutable_simulation_status()->set_is_alive(true);
  return agent_status;
}

void SimulationWorld::Initialize() { AddVehicleAgent(simulation_config_.agent_name()); }

void SimulationWorld::AddVehicleAgent(const std::string& name) {
  interface::agent::AgentStatus agent_status;
  agent_status.CopyFrom(GenerateInitialStatusFromRoute());
  std::unique_ptr<vehicle_status_model::VehicleStatusModelSolver> solver =
      vehicle_status_model::CreateVehicleStatusModelSolver(vehicle_params_);
  solver->Initialize(0, agent_status.vehicle_status());
  dynamic_solver_map_[name] = std::move(solver);
  agent_status_map_[name].CopyFrom(agent_status);
}

void SimulationWorld::UpdateTripInfo() {
  for (auto& iter : agent_status_map_) {
    const std::string& name = iter.first;
    interface::agent::AgentStatus& agent_status = iter.second;
    if (agent_status.simulation_status().is_alive()) {
      if (DetermineReachedDestination(agent_status.vehicle_status(), agent_status.route_status())) {
        LOG(ERROR) << "Agent:" << name << "reached the destination!";
        agent_status.clear_route_status();
        agent_status.mutable_simulation_status()->set_is_finished(true);
      }
    }
  }
}

void SimulationWorld::UpdateVehicleStatus() {
  for (auto& iter : agent_status_map_) {
    const std::string& name = iter.first;
    const interface::agent::AgentStatus& agent_status = iter.second;
    const interface::agent::SimulationStatus& simulation_status = agent_status.simulation_status();
    if (simulation_status.is_alive()) {
      agent_status_map_.at(name).mutable_vehicle_status()->CopyFrom(
          dynamic_solver_map_[name]->UpdateVehicleStatus(current_time_,
                                                         control_command_map_[name]));
      const std::string& name = iter.first;
      CHECK(control_command_map_.find(name) != control_command_map_.end());
    }
  }
}

void SimulationWorld::UpdateSimulationTime() {
  for (auto& iter : agent_status_map_) {
    iter.second.mutable_simulation_status()->set_simulation_time(current_time_);
  }
}

void SimulationWorld::RunOneIteration() {
  UpdateVehicleStatus();
  UpdateTripInfo();
  current_time_ += 0.01;
  UpdateSimulationTime();
}

bool SimulationWorld::DetermineSimulationFinished() {
  for (auto& iter : agent_status_map_) {
    const interface::agent::AgentStatus& agent_status = iter.second;
    if (!agent_status.simulation_status().is_finished()) {
      return false;
    }
  }
  return true;
}

bool SimulationWorld::DetermineReachedDestination(const interface::agent::VehicleStatus& vehicle_status,
                                 const interface::agent::RouteStatus& route_status) {
  math::Vec2d pos_in_router_request(route_status.destination().x(),
                                    route_status.destination().y());
  math::Vec2d current_pos(vehicle_status.position().x(), vehicle_status.position().y());
  math::Vec3d current_velocity(vehicle_status.velocity().x(), vehicle_status.velocity().y(),
                               vehicle_status.velocity().z());
  return pos_in_router_request.DistanceToPoint(current_pos) < 2.0 &&
      current_velocity.Length() < 0.3;
}

};  // namespace simulation

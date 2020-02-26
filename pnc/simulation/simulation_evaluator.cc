// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/simulation_evaluator.h"

namespace {

constexpr double kLaneWidth = 5.0;

// If the point is in front of the stop line, return 1
int GetSide(double point_x, double point_y, double line_x0, double line_y0, double line_x1,
            double line_y1) {
  math::Vec2d vec1(point_x - line_x0, point_y - line_y0);
  math::Vec2d vec2(line_x1 - line_x0, line_y1 - line_y0);
  double outer_product = vec1.OuterProd(vec2);
  if (std::abs(outer_product) < math::kEpsilon) {
    return 0;
  } else {
    if (outer_product < 0) {
      return -1;
    } else {
      return 1;
    }
  }
}

}  // namespace anonymous

namespace simulation {

using interface::map::TrafficLight;
using math::Vec2d;
using pnc::map::Segment;
using utils::Optional;
using utils::none;

void SimulationEvaluator::Initialize(const AgentStatusMap& agent_status_map) {
  for (const auto& iter : agent_status_map) {
    metric_collector_map_[iter.first] = std::make_unique<MetricCollector>(iter.first);
    metric_collector_map_[iter.first]->AddMetricFrame(iter.second);
  }
}

void SimulationEvaluator::EvaluateOneIteration(const AgentResponseMap& agent_response_map,
                                               const std::vector<math::Box2d>& pedestrian_boxes,
                                               AgentStatusMap* agent_status_map) {
  EvaluateVehicleExceedSpeedLimit(agent_status_map);
  EvaluateCollision(pedestrian_boxes, agent_status_map);
  EvaluateOutOfLane(agent_status_map);
  EvaluateRedLightRunning(agent_status_map);
  EvaluateIterationTimeout(agent_response_map, agent_status_map);
  for (const auto& iter : *agent_status_map) {
    metric_collector_map_[iter.first]->AddMetricFrame(iter.second);
  }
}

void SimulationEvaluator::UpdateTrafficLightStatus(
    const interface::perception::PerceptionTrafficLightStatus& traffic_light_status) {
  traffic_light_status_.CopyFrom(traffic_light_status);
  traffic_light_color_map_.clear();
  for (const auto& single_traffic_light_status :
       traffic_light_status_.single_traffic_light_status()) {
    traffic_light_color_map_[single_traffic_light_status.id().id()] =
        single_traffic_light_status.color();
  }
  vehicle_in_front_of_stop_line_map_.clear();
}

interface::simulation::SimulationMetrics SimulationEvaluator::GetSimulationMetric() {
  interface::simulation::SimulationMetrics simulation_metric;
  for (auto& iter : metric_collector_map_) {
    simulation_metric.add_agent_metric()->CopyFrom(iter.second->DumpMetric());
  }
  return simulation_metric;
}

void SimulationEvaluator::EvaluateVehicleExceedSpeedLimit(AgentStatusMap* agent_status_map) {
  if (!simulation_config_.rule_config().has_vehicle_exceed_speed_limit_rule()) {
    return;
  }
  double speed_limit =
      simulation_config_.rule_config().vehicle_exceed_speed_limit_rule().hard_speed_limit_kmh() /
      3.6;
  for (auto& iter : *agent_status_map) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    interface::agent::AgentStatus& agent_status = iter.second;
    math::Vec3d velocity(agent_status.vehicle_status().velocity().x(),
                         agent_status.vehicle_status().velocity().y(),
                         agent_status.vehicle_status().velocity().z());
    if (velocity.Length() > speed_limit) {
      agent_status.mutable_simulation_status()->set_is_alive(false);
      agent_status.mutable_simulation_status()->set_elimination_reason(
          "Velocity exceeds speed limit: " + std::to_string(speed_limit));
    }
  }
}

void SimulationEvaluator::EvaluateOutOfLane(AgentStatusMap* agent_status_map) {
  if (!simulation_config_.rule_config().check_vehicle_out_of_lane()) {
    return;
  }
  for (auto& iter : *agent_status_map) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    double x = iter.second.vehicle_status().position().x();
    double y = iter.second.vehicle_status().position().y();
    bool out_of_lane = false;

    Optional<Segment> nearest_central_line = map_meta_->GetNearestCentralLineSegment(x, y);
    if (nearest_central_line == none ||
        nearest_central_line.get().DistanceToPoint(Vec2d(x, y)) > kLaneWidth / 2 + 0.5) {
      out_of_lane = true;
    }

    if (out_of_lane) {
      iter.second.mutable_simulation_status()->set_is_alive(false);
      iter.second.mutable_simulation_status()->set_elimination_reason("Out of lane");
    }
  }
}

void SimulationEvaluator::EvaluateRedLightRunning(AgentStatusMap* agent_status_map) {
  if (!simulation_config_.rule_config().check_run_red_traffic_light()) {
    return;
  }
  for (auto& iter : *agent_status_map) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    const std::string& vehicle_name = iter.first;
    double x = iter.second.vehicle_status().position().x();
    double y = iter.second.vehicle_status().position().y();

    Optional<TrafficLight> nearest_traffic_light = map_meta_->GetNearestTrafficLight(x, y);

    bool running_red_light = false;

    if (nearest_traffic_light != none) {
      const std::string& traffic_light_id = nearest_traffic_light.get().id().id();
      const auto& stop_line = nearest_traffic_light.get().stop_line();
      if (traffic_light_color_map_[traffic_light_id] != interface::map::Bulb::RED) {
        continue;
      }
      math::Vec2d stop_line_heading_unit =
          math::Vec2d(stop_line.point(1).x() - stop_line.point(0).x(),
                      stop_line.point(1).y() - stop_line.point(0).y())
              .Rotate90();
      math::Vec2d vehicle_heading_unit = math::Vec2d::FromUnit(math::transform::GetYaw(
          math::transform::ToEigen(iter.second.vehicle_status().orientation())));
      // If vehicle is not heading to traffic light, not evaluate red-light running
      if (vehicle_heading_unit.InnerProd(stop_line_heading_unit) < 0) {
        continue;
      }

      pnc::map::Segment segment(math::Vec2d(stop_line.point(0).x(), stop_line.point(0).y()),
                                math::Vec2d(stop_line.point(1).x(), stop_line.point(1).y()));
      if (segment.DistanceToPoint(math::Vec2d(x, y)) < 0.1) {
        continue;
      }

      int side = GetSide(x, y, stop_line.point(0).x(), stop_line.point(0).y(),
                         stop_line.point(1).x(), stop_line.point(1).y());
      if (side > 0) {
        vehicle_in_front_of_stop_line_map_[vehicle_name] = traffic_light_id;
      }
      if (side < 0) {
        if (vehicle_in_front_of_stop_line_map_[vehicle_name] == traffic_light_id) {
          running_red_light = true;
        }
      }
    }

    if (running_red_light) {
      iter.second.mutable_simulation_status()->set_is_alive(false);
      iter.second.mutable_simulation_status()->set_elimination_reason(
          "Running red light: " + nearest_traffic_light.get().id().id());
    }
  }
}

void SimulationEvaluator::EvaluateCollision(const std::vector<math::Box2d>& pedestrian_boxes,
                                            AgentStatusMap* agent_status_map) {
  if (!simulation_config_.rule_config().check_vehicles_collision()) {
    return;
  }
  for (auto& iter1 : *agent_status_map) {
    if (!iter1.second.simulation_status().is_alive()) {
      continue;
    }
    for (int i = 0; i < pedestrian_boxes.size(); i++) {
      if (InCollision(pedestrian_boxes[i], iter1.second.vehicle_status())) {
        iter1.second.mutable_simulation_status()->set_is_alive(false);
        iter1.second.mutable_simulation_status()->set_elimination_reason(
            "Collision with pedestrian P" + std::to_string(i));
        break;
      }
    }
    if (!iter1.second.simulation_status().is_alive()) {
      continue;
    }
    for (auto& iter2 : *agent_status_map) {
      if (!iter2.second.simulation_status().is_alive()) {
        continue;
      }
      if (iter1.first == iter2.first) {
        continue;
      }
      if (InCollision(iter1.second.vehicle_status(), iter2.second.vehicle_status())) {
        if (RearEnd(iter1.second.vehicle_status(), iter2.second.vehicle_status())) {
          iter1.second.mutable_simulation_status()->set_is_alive(false);
          iter1.second.mutable_simulation_status()->set_elimination_reason(
              "Rear-end collision with " + iter2.first);
        } else {
          if (RearEnd(iter2.second.vehicle_status(), iter1.second.vehicle_status())) {
            iter2.second.mutable_simulation_status()->set_is_alive(false);
            iter2.second.mutable_simulation_status()->set_elimination_reason(
                "Rear-end collision with " + iter1.first);
          } else {
            iter1.second.mutable_simulation_status()->set_is_alive(false);
            iter1.second.mutable_simulation_status()->set_elimination_reason("Collision with " +
                                                                             iter2.first);
            iter2.second.mutable_simulation_status()->set_is_alive(false);
            iter2.second.mutable_simulation_status()->set_elimination_reason("Collision with " +
                                                                             iter1.first);
          }
        }
      }
    }
  }
}

// Check if vehicle 1 hit vehicle 2's tail
bool SimulationEvaluator::RearEnd(const interface::agent::VehicleStatus& vehicle_status1,
                                  const interface::agent::VehicleStatus& vehicle_status2) {
  math::Box2d box1 = utils::vehicle::GetVehicleBox(vehicle_status1, vehicle_params_);
  math::Box2d box2 = utils::vehicle::GetVehicleBox(vehicle_status2, vehicle_params_);
  math::Vec2d center_vec = box2.center() - box1.center();
  double center_vec_heading = std::atan2(center_vec.y, center_vec.x);

  return std::abs(math::NormalizeAngle(box2.heading() - box1.heading())) <
             math::DegreeToRadian(20) &&
         std::abs(math::NormalizeAngle(center_vec_heading - box1.heading())) <
             math::DegreeToRadian(20);
}

bool SimulationEvaluator::InCollision(const interface::agent::VehicleStatus& vehicle_status1,
                                      const interface::agent::VehicleStatus& vehicle_status2) {
  math::Box2d box1 = utils::vehicle::GetVehicleBox(vehicle_status1, vehicle_params_);
  math::Box2d box2 = utils::vehicle::GetVehicleBox(vehicle_status2, vehicle_params_);
  return box1.HasOverlapWithBox(box2);
}

bool SimulationEvaluator::InCollision(const math::Box2d& box1,
                                      const interface::agent::VehicleStatus& vehicle_status) {
  math::Box2d box2 = utils::vehicle::GetVehicleBox(vehicle_status, vehicle_params_);
  return box1.HasOverlapWithBox(box2);
}

void SimulationEvaluator::EvaluateIterationTimeout(const AgentResponseMap& agent_response_map,
                                                   AgentStatusMap* agent_status_map) {
  if (!simulation_config_.rule_config().has_one_iteration_timeout_rule()) {
    return;
  }
  for (auto& iter : *agent_status_map) {
    if (!iter.second.simulation_status().is_alive()) {
      continue;
    }
    const std::string& name = iter.first;
    bool hit_hard_timeout = false;
    if (agent_response_map.find(name) == agent_response_map.end()) {
      hit_hard_timeout = true;
    } else {
      if (agent_response_map.at(name).iteration_time_cost() >
          simulation_config_.rule_config().one_iteration_timeout_rule().hard_iteration_timeout()) {
        hit_hard_timeout = true;
      } else {
        if (agent_response_map.at(name).iteration_time_cost() > simulation_config_.rule_config()
                                                                    .one_iteration_timeout_rule()
                                                                    .soft_iteration_timeout()) {
          soft_limit_hit_map_[name]++;
          LOG(INFO) << "Agent:" << name << " hit soft timeout limit";
        }
      }
    }
    if (hit_hard_timeout) {
      LOG(INFO) << "Agent:" << name << " hit hard timeout limit";
      iter.second.mutable_simulation_status()->set_is_alive(false);
      iter.second.mutable_simulation_status()->set_elimination_reason(
          "Iteration time cost hit hard timeout threshold");
      continue;
    }
    if (soft_limit_hit_map_[name] > simulation_config_.rule_config()
                                        .one_iteration_timeout_rule()
                                        .allowed_num_iterations_soft_timeout()) {
      iter.second.mutable_simulation_status()->set_is_alive(false);
      iter.second.mutable_simulation_status()->set_elimination_reason(
          "Too many slow iterations that hit soft timeout threshold");
      continue;
    }
  }
}

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/mocked_obstacle_agent.h"

namespace simulation {

namespace {

constexpr double kLength = 0.5;
constexpr double kWidth = 0.5;

unsigned int seed = 2018;

double RandomDouble() { return static_cast<double>(rand_r(&seed)) / static_cast<double>(RAND_MAX); }

double RandomDouble(double min_value, double max_value) {
  CHECK_LE(min_value, max_value);
  return RandomDouble() * (max_value - min_value) + min_value;
}
}

math::Vec2d MockedObstacleAgent::GenerateRandomCenter() {
  int k = std::rand() % 4;
  math::Vec2d center;
  if (k == 0) {
    center = math::Vec2d(min_x_, RandomDouble(min_y_, max_y_));
  }
  if (k == 1) {
    center = math::Vec2d(max_x_, RandomDouble(min_y_, max_y_));
  }
  if (k == 2) {
    center = math::Vec2d(RandomDouble(min_x_, max_x_), min_y_);
  }
  if (k == 3) {
    center = math::Vec2d(RandomDouble(min_x_, max_x_), max_y_);
  }
  return center;
}

void MockedObstacleAgent::Initialize() {
  for (int i = 0; i < kNumPedestrianAgents; i++) {
    centers_.push_back(GenerateRandomCenter());
    headings_.push_back(RandomDouble(0, 2 * M_PI));
    // speed from 3km/h to 6km/h
    speeds_.push_back(RandomDouble(3.0, 6.0) / 3.6);
    stop_moving_count_.push_back(0);
  }
}

std::vector<math::Box2d> MockedObstacleAgent::RunOneIteration(
    const AgentStatusMap& agent_status_map) {
  std::vector<math::Box2d> result;
  for (int i = 0; i < kNumPedestrianAgents; i++) {
    math::Vec2d new_center =
        centers_[i] +
        math::Vec2d(std::cos(headings_[i]), std::sin(headings_[i])) * speeds_[i] * 0.5;
    // Near the map boundary, change the heading
    if (new_center.x < min_x_ || new_center.x > max_x_ || new_center.y < min_y_ ||
        new_center.y > max_y_) {
      headings_[i] = RandomDouble(0, 2 * M_PI);
      continue;
    }
    // Expected to change heading every 30 seconds
    if (std::rand() % 3000 == 0) {
      headings_[i] = RandomDouble(0, 2 * M_PI);
      continue;
    }
    // Predict the center of the pedestrian after one second
    std::vector<math::Vec2d> future_centers;
    for (int times = 1; times <= 4; times++) {
      future_centers.push_back(centers_[i] +
                               math::Vec2d(std::cos(headings_[i]), std::sin(headings_[i])) *
                                   speeds_[i] * (0.25 * times));
    }
    bool near_vehicle = false;
    for (auto& iter : agent_status_map) {
      if (!iter.second.simulation_status().is_alive()) {
        continue;
      }
      const auto& vehicle_status = iter.second.vehicle_status();
      math::Box2d vehicle_box = utils::vehicle::GetVehicleBox(vehicle_status, vehicle_params_);
      for (const auto& future_center : future_centers) {
        if (vehicle_box.HasOverlapWithBox(
                math::Box2d(future_center, headings_[i], kLength + 0.3, kWidth + 0.3))) {
          near_vehicle = true;
          break;
        }
      }
      if (near_vehicle) {
        break;
      }
    }
    if (!near_vehicle) {
      // If pedestrian is not near vehicle, update the position
      centers_[i] =
          centers_[i] +
          math::Vec2d(std::cos(headings_[i]), std::sin(headings_[i])) * speeds_[i] * kIterationTime;
    } else {
      // If pedestrian is near vehicle, stop moving
      stop_moving_count_[i]++;
      // If stop moving for 5 seconds, change heading
      if (stop_moving_count_[i] == 500) {
        stop_moving_count_[i] = 0;
        headings_[i] = RandomDouble(0, 2 * M_PI);
      }
    }
  }
  for (int i = 0; i < kNumPedestrianAgents; i++) {
    result.push_back(math::Box2d(centers_[i], headings_[i], kLength, kWidth));
  }
  return result;
}

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "pnc/simulation/vehicle_agent.h"

#include "common/proto/agent_status.pb.h"
#include "common/proto/geometry.pb.h"
#include "common/proto/vehicle_status.pb.h"
#include "common/utils/math/math_utils.h"
#include "pnc/simulation/vehicle_agent_factory.h"

namespace sample {

// A sample vehicle agent for route 1
// This agent will always run in straight. It will accelerate to the speed slightly over 5m/s,
// and keeps running with the speed over 5m/s until reaches destination.

class SampleVehicleAgent : public simulation::VehicleAgent {
 public:
  explicit SampleVehicleAgent(const std::string& name) : VehicleAgent(name) {}

  void Initialize(const interface::agent::AgentStatus& /* agent_status */) override {
    // Nothing to initialize
  }

  interface::control::ControlCommand RunOneIteration(
      const interface::agent::AgentStatus& agent_status) override {
    interface::control::ControlCommand command;
    // Vehicle's current position reaches the destination
    if (CalcDistance(agent_status.vehicle_status().position(),
                     agent_status.route_status().destination()) < 3.0) {
      position_reached_destination_ = true;
    }
    // Vehicle's current velocity reaches 5 m/s
    if (CalcVelocity(agent_status.vehicle_status().velocity()) > 5) {
      velocity_reached_threshold_ = true;
    }

    if (position_reached_destination_) {
      // Set maximum brake ratio to stop the vehicle
      command.set_brake_ratio(1.0);
    } else {
      if (!velocity_reached_threshold_) {
        // Set throttle ratio to accelerate
        command.set_throttle_ratio(0.3);
      }
    }

    PublishVariable("key1", "var1");
    PublishVariable("key2", "var2", utils::display::Color::Red());
    PublishVariable("key3", "var3", utils::display::Color::Red(), utils::display::Color::Green());

    return command;
  }

 private:
  double CalcDistance(const interface::geometry::Vector3d& position,
                      const interface::geometry::Point3D& destination) {
    double sqr_sum =
        math::Sqr(position.x() - destination.x()) + math::Sqr(position.y() - destination.y());
    return std::sqrt(sqr_sum);
  }

  double CalcVelocity(const interface::geometry::Vector3d& velocity) {
    double sqr_sum = math::Sqr(velocity.x()) + math::Sqr(velocity.y());
    return std::sqrt(sqr_sum);
  }

  // Whether vehicle's current position reaches the destination
  bool position_reached_destination_ = false;
  // Whether vehicle's current velocity reaches 5 m/s
  bool velocity_reached_threshold_ = false;
};

}  // namespace sample

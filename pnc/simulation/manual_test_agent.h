// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/common/mutex.h"
#include "pnc/simulation/vehicle_agent.h"

namespace {

constexpr double kSteeringAngleDelta = 0.3;
constexpr double kPedalThrottleDelta = 0.2;

}  // namespace anonymous

namespace simulation {

constexpr char kKeyboardControlledAgentName[] = "KeyboardTest";

class ManualTestAgent : public VehicleAgent {
 public:
  ManualTestAgent() : VehicleAgent(std::string(kKeyboardControlledAgentName)) {
    utils::MutexLocker lock(&mutex_);
    brake_throttle_ = 0.0;
    steering_angle_ = 0.0;
    last_steering_timestamp = 0.0;
  }

  void Initialize(const interface::agent::AgentStatus& /*agent_status*/) override {}

  interface::control::ControlCommand RunOneIteration(
      const interface::agent::AgentStatus& agent_status) override {
    current_time_ = agent_status.simulation_status().simulation_time();
    interface::control::ControlCommand control_command;
    {
      utils::MutexLocker lock(&mutex_);
      if (current_time_ - last_steering_timestamp > 0.1) {
        steering_angle_ = 0.0;
        last_steering_timestamp = current_time_;
      }
      if (current_time_ - last_throttle_timestamp > 0.1) {
        brake_throttle_ = 0.0;
        last_throttle_timestamp = current_time_;
      }
      if (brake_throttle_ > 0) {
        control_command.set_throttle_ratio(brake_throttle_);
      } else {
        control_command.set_brake_ratio(std::abs(brake_throttle_));
      }
      control_command.set_steering_angle(steering_angle_);
    }
    return control_command;
  }

  void TurnLeft() {
    utils::MutexLocker lock(&mutex_);
    if (steering_angle_ < 0) {
      steering_angle_ = 0;
    }
    steering_angle_ += kSteeringAngleDelta;
    last_steering_timestamp = current_time_;
  }

  void TurnRight() {
    utils::MutexLocker lock(&mutex_);
    if (steering_angle_ > 0) {
      steering_angle_ = 0;
    }
    steering_angle_ -= kSteeringAngleDelta;
    last_steering_timestamp = current_time_;
  }

  void SpeedUp() {
    utils::MutexLocker lock(&mutex_);
    brake_throttle_ += kPedalThrottleDelta;
    brake_throttle_ = std::min(1.0, brake_throttle_);
  }

  void SlowDown() {
    utils::MutexLocker lock(&mutex_);
    brake_throttle_ -= kPedalThrottleDelta;
    brake_throttle_ = std::max(-1.0, brake_throttle_);
  }

  void Stop() {
    utils::MutexLocker lock(&mutex_);
    brake_throttle_ = -1.0;
  }

 private:
  utils::Mutex mutex_{"ManualTestAgent"};

  double brake_throttle_ GUARDED_BY(mutex_);
  double steering_angle_ GUARDED_BY(mutex_);

  double last_steering_timestamp GUARDED_BY(mutex_);
  double last_throttle_timestamp GUARDED_BY(mutex_);

  double current_time_;
};

};  // namespace simulation

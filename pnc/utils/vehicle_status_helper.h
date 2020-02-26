// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "Eigen/Core"
#include "Eigen/Dense"

#include "common/proto/perception.pb.h"
#include "common/utils/math/transform/transform.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

#include "common/proto/agent_status.pb.h"

namespace utils {
namespace vehicle {

class VehicleStatusHelper {
 public:
  explicit VehicleStatusHelper(const interface::agent::VehicleStatus& proto);
  VehicleStatusHelper(const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation,
                      const Eigen::Vector3d& velocity, const Eigen::Vector3d& angular_velocity_vcs,
                      const Eigen::Vector3d& acceleration_vcs);

  // The origin of Vehicle Coordinate System in Smooth Coordinate System, in meters
  const Eigen::Vector3d& position() const { return position_; }

  const math::Vec2d position2d() const { return math::Vec2d(position_.x(), position_.y()); }
  // The orientation of Vehicle Coordinate System in Smooth Coordinate System
  const Eigen::Quaterniond& orientation() const { return orientation_; }

  // The orientation of Driving Coordinate System in Smooth Coordinate System
  Eigen::Quaterniond driving_orientation() const;
  // The angle of the x axis of Vehicle Coordinate System
  // in the xy-plane of Smooth Coordinate System
  double heading() const;
  // The angle of the x axis of Driving Coordinate System
  // in the xy-plane of Smooth Coordinate System
  double driving_heading() const;
  // The position and orientation of Vehicle Coordinate System in Smooth Coordinate System
  math::transform::Rigid3d pose() const;
  // The linear velocity of the origin of Vehicle Coordinate System in Smooth Coordinate System
  // in m/s
  const Eigen::Vector3d& velocity() const { return velocity_; }
  // velocity() in Vehicle Coordinate System
  Eigen::Vector3d velocity_vcs() const;
  // The angular velocity of the origin of Vehicle Coordinate System in Smooth Coordinate System,
  // in rad/s, not including the earth's rotation
  Eigen::Vector3d angular_velocity() const;
  // angular_velocity() in Vehicle Coordinate System
  const Eigen::Vector3d& angular_velocity_vcs() const { return angular_velocity_vcs_; }
  // The acceleration of the origin of Vehicle Coordinate System in Smooth Coordinate System,
  // in m/s, not including the gravitational acceleration
  Eigen::Vector3d acceleration() const;
  // acceleration() in Vehicle Coordinate System
  const Eigen::Vector3d& acceleration_vcs() const { return acceleration_vcs_; }

  interface::agent::VehicleStatus ToProto() const;

 private:
  Eigen::Vector3d position_ = Eigen::Vector3d::Zero();
  Eigen::Quaterniond orientation_ = Eigen::Quaterniond::Identity();
  Eigen::Vector3d velocity_ = Eigen::Vector3d::Zero();
  Eigen::Vector3d angular_velocity_vcs_ = Eigen::Vector3d::Zero();
  Eigen::Vector3d acceleration_vcs_ = Eigen::Vector3d::Zero();
};

}  // namespace vehicle
}  // namespace utils

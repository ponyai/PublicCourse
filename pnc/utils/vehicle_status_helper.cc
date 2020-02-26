// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/utils/vehicle_status_helper.h"

namespace utils {
namespace vehicle {

using math::transform::ToEigen;
using math::transform::ToProto;

VehicleStatusHelper::VehicleStatusHelper(const interface::agent::VehicleStatus& pb)
    : position_(ToEigen(pb.position())),
      orientation_(ToEigen(pb.orientation())),
      velocity_(ToEigen(pb.velocity())),
      angular_velocity_vcs_(ToEigen(pb.angular_velocity_vcs())),
      acceleration_vcs_(ToEigen(pb.acceleration_vcs())) {}

VehicleStatusHelper::VehicleStatusHelper(const Eigen::Vector3d& position,
                                         const Eigen::Quaterniond& orientation,
                                         const Eigen::Vector3d& velocity,
                                         const Eigen::Vector3d& angular_velocity_vcs,
                                         const Eigen::Vector3d& acceleration_vcs)
    : position_(position),
      orientation_(orientation),
      velocity_(velocity),
      angular_velocity_vcs_(angular_velocity_vcs),
      acceleration_vcs_(acceleration_vcs) {}

interface::agent::VehicleStatus VehicleStatusHelper::ToProto() const {
  interface::agent::VehicleStatus pb;
  *pb.mutable_position() = math::transform::ToProto(position_);
  *pb.mutable_orientation() = math::transform::ToProto(orientation_);
  *pb.mutable_velocity() = math::transform::ToProto(velocity_);
  *pb.mutable_angular_velocity_vcs() = math::transform::ToProto(angular_velocity_vcs_);
  *pb.mutable_acceleration_vcs() = math::transform::ToProto(acceleration_vcs_);
  return pb;
}

double VehicleStatusHelper::driving_heading() const {
  return math::transform::GetYaw(driving_orientation());
}

double VehicleStatusHelper::heading() const { return math::transform::GetYaw(orientation_); }

math::transform::Rigid3d VehicleStatusHelper::pose() const {
  return math::transform::Rigid3d(position_, orientation_);
}

Eigen::Vector3d VehicleStatusHelper::velocity_vcs() const {
  return orientation_.inverse() * velocity_;
}

Eigen::Vector3d VehicleStatusHelper::angular_velocity() const {
  return orientation_ * angular_velocity_vcs_;
}

Eigen::Vector3d VehicleStatusHelper::acceleration() const {
  return orientation_ * acceleration_vcs_;
}

Eigen::Quaterniond VehicleStatusHelper::driving_orientation() const { return orientation_; }

}  // namespace vehicle
}  // namespace utils

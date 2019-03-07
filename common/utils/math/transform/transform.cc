/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/utils/math/transform/transform.h"

namespace math {
namespace transform {

double GetYaw(const interface::geometry::Quaterniond& rotation) {
  return GetYaw(ToEigen(rotation));
}

Eigen::Quaterniond RollPitchYaw(double roll, double pitch, double yaw) {
  const Eigen::AngleAxisd roll_angle(roll, Eigen::Vector3d::UnitX());
  const Eigen::AngleAxisd pitch_angle(pitch, Eigen::Vector3d::UnitY());
  const Eigen::AngleAxisd yaw_angle(yaw, Eigen::Vector3d::UnitZ());
  return yaw_angle * pitch_angle * roll_angle;
}

Eigen::Vector3f ToEigen(const interface::geometry::Vector3f& vector) {
  return Eigen::Vector3f(vector.x(), vector.y(), vector.z());
}

Eigen::Vector3d ToEigen(const interface::geometry::Vector3d& vector) {
  return Eigen::Vector3d(vector.x(), vector.y(), vector.z());
}

Eigen::Vector3d ToEigen(const interface::geometry::Point3D& point) {
  return Eigen::Vector3d(point.x(), point.y(), point.z());
}

Eigen::Quaterniond ToEigen(const interface::geometry::Quaterniond& quaternion) {
  return Eigen::Quaterniond(quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
}

Eigen::Quaterniond ToEigen(const interface::geometry::EulerAnglesd& euler_angle) {
  return RollPitchYaw(euler_angle.roll(), euler_angle.pitch(), euler_angle.yaw());
}

interface::geometry::Rigid3d ToProto(const Rigid3d& rigid) {
  interface::geometry::Rigid3d proto;
  *proto.mutable_translation() = ToProto(rigid.translation());
  *proto.mutable_rotation() = ToProto(rigid.rotation());
  return proto;
}

Rigid3d ToRigid3(const interface::geometry::Rigid3d& rigid) {
  if (rigid.has_rotation_euler()) {
    return Rigid3d(ToEigen(rigid.translation()), ToEigen(rigid.rotation_euler()));
  }
  return Rigid3d(ToEigen(rigid.translation()), ToEigen(rigid.rotation()));
}

interface::geometry::Rigid3f ToProto(const Rigid3f& rigid) {
  interface::geometry::Rigid3f proto;
  *proto.mutable_translation() = ToProto(rigid.translation());
  *proto.mutable_rotation() = ToProto(rigid.rotation());
  return proto;
}

interface::geometry::Vector3f ToProto(const Eigen::Vector3f& vector) {
  interface::geometry::Vector3f proto;
  proto.set_x(vector.x());
  proto.set_y(vector.y());
  proto.set_z(vector.z());
  return proto;
}

interface::geometry::Vector3d ToProto(const Eigen::Vector3d& vector) {
  interface::geometry::Vector3d proto;
  proto.set_x(vector.x());
  proto.set_y(vector.y());
  proto.set_z(vector.z());
  return proto;
}

interface::geometry::Quaternionf ToProto(const Eigen::Quaternionf& quaternion) {
  interface::geometry::Quaternionf proto;
  proto.set_w(quaternion.w());
  proto.set_x(quaternion.x());
  proto.set_y(quaternion.y());
  proto.set_z(quaternion.z());
  return proto;
}

interface::geometry::Quaterniond ToProto(const Eigen::Quaterniond& quaternion) {
  interface::geometry::Quaterniond proto;
  proto.set_w(quaternion.w());
  proto.set_x(quaternion.x());
  proto.set_y(quaternion.y());
  proto.set_z(quaternion.z());
  return proto;
}

interface::geometry::Vector3d ToVector3d(const math::Vec2d& vec2d) {
  interface::geometry::Vector3d vector3d;
  vector3d.set_x(vec2d.x);
  vector3d.set_y(vec2d.y);
  return vector3d;
}

}  // namespace transform
}  // namespace math

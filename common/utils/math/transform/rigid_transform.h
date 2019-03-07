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

#pragma once

#include <string>

#include <glog/logging.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace math {
namespace transform {

// The linear and angular position of a 3D rigid body
template <typename FloatType>
class Rigid3 {
 public:
  using Affine = Eigen::Transform<FloatType, 3, Eigen::Affine>;
  using Vector = Eigen::Matrix<FloatType, 3, 1>;
  using Quaternion = Eigen::Quaternion<FloatType>;
  using AngleAxis = Eigen::AngleAxis<FloatType>;

  Rigid3() : translation_(Vector::Zero()), rotation_(Quaternion::Identity()) {}

  explicit Rigid3(const Affine& affine)
      : translation_(affine.translation()), rotation_(affine.rotation()) {}

  Rigid3(const Vector& translation, const Quaternion& rotation)
      : translation_(translation), rotation_(rotation) {}

  Rigid3(const Vector& translation, const AngleAxis& rotation)
      : translation_(translation), rotation_(rotation) {}

  Rigid3(FloatType x, FloatType y, FloatType z, FloatType qx, FloatType qy, FloatType qz,
         FloatType qw)
      : rotation_(qw, qx, qy, qz) {
    translation_ << x, y, z;
  }

  // Flattened rigid transform: x, y, z, qx, qy, qz, qw.
  explicit Rigid3(const FloatType* flatten) {
    CHECK(flatten != nullptr);
    rotation_ = Quaternion(flatten[6], flatten[3], flatten[4], flatten[5]);
    translation_ << flatten[0], flatten[1], flatten[2];
  }

  static Rigid3 Rotation(const AngleAxis& angle_axis) {
    return Rigid3(Vector::Zero(), Quaternion(angle_axis));
  }

  static Rigid3 Rotation(const Quaternion& rotation) { return Rigid3(Vector::Zero(), rotation); }

  static Rigid3 Translation(const Vector& vector) { return Rigid3(vector, Quaternion::Identity()); }

  static Rigid3<FloatType> Identity() {
    return Rigid3<FloatType>(Vector::Zero(), Quaternion::Identity());
  }

  template <typename OtherType>
  Rigid3<OtherType> cast() const {
    return Rigid3<OtherType>(translation_.template cast<OtherType>(),
                             rotation_.template cast<OtherType>());
  }

  Affine ToAffine() const {
    return Eigen::Translation<FloatType, 3>(translation_.x(), translation_.y(), translation_.z()) *
           rotation_;
  }

  const Vector& translation() const { return translation_; }

  const Quaternion& rotation() const { return rotation_; }

  Vector* mutable_translation() { return &translation_; }

  Quaternion* mutable_rotation() { return &rotation_; }

  Rigid3 inverse() const {
    const Quaternion rotation = rotation_.conjugate();
    const Vector translation = -(rotation * translation_);
    return Rigid3(translation, rotation);
  }

  void Flatten(FloatType* flatten) const {
    CHECK(flatten != nullptr);
    flatten[0] = translation_.x();
    flatten[1] = translation_.y();
    flatten[2] = translation_.z();
    flatten[3] = rotation_.x();
    flatten[4] = rotation_.y();
    flatten[5] = rotation_.z();
    flatten[6] = rotation_.w();
  }

  std::string DebugString() const {
    std::string out;
    out.append("{ t: [");
    out.append(std::to_string(translation().x()));
    out.append(", ");
    out.append(std::to_string(translation().y()));
    out.append(", ");
    out.append(std::to_string(translation().z()));
    out.append("], q: [");
    out.append(std::to_string(rotation().w()));
    out.append(", ");
    out.append(std::to_string(rotation().x()));
    out.append(", ");
    out.append(std::to_string(rotation().y()));
    out.append(", ");
    out.append(std::to_string(rotation().z()));
    out.append("] }");
    return out;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

 private:
  Vector translation_;
  Quaternion rotation_;
};

template <typename FloatType>
Rigid3<FloatType> operator*(const Rigid3<FloatType>& lhs, const Rigid3<FloatType>& rhs) {
  return Rigid3<FloatType>(lhs.rotation() * rhs.translation() + lhs.translation(),
                           (lhs.rotation() * rhs.rotation()).normalized());
}

template <typename FloatType>
typename Rigid3<FloatType>::Vector operator*(const Rigid3<FloatType>& rigid,
                                             const typename Rigid3<FloatType>::Vector& point) {
  return rigid.rotation() * point + rigid.translation();
}

// This is needed for gmock.
template <typename T>
std::ostream& operator<<(std::ostream& os, const Rigid3<T>& rigid) {
  os << rigid.DebugString();
  return os;
}

// Rigid3d -> glm::mat4
// Rigid3f -> glm::dmat4
template <typename FloatType>
typename glm::tmat4x4<FloatType> ToGlm(const Rigid3<FloatType>& rigid) {
  using GlmMat4 = typename glm::tmat4x4<FloatType>;
  using GlmVec3 = typename glm::tvec3<FloatType>;

  const Eigen::AngleAxis<FloatType> rotation(rigid.rotation());
  const GlmVec3 axis(rotation.axis().x(), rotation.axis().y(), rotation.axis().z());
  const GlmMat4 r = glm::rotate(GlmMat4(1.0), rotation.angle(), axis);

  const Eigen::Matrix<FloatType, 3, 1> translation = rigid.translation();
  const GlmMat4 t =
      glm::translate(GlmMat4(1.0), GlmVec3(translation.x(), translation.y(), translation.z()));
  return t * r;
}

// glm::mat4 -> Rigid3d
// glm::dmat4 -> Rigid3f
template <typename FloatType>
Rigid3<FloatType> ToRigid(const glm::tmat4x4<FloatType>& transform) {
  const Eigen::Map<const Eigen::Matrix<FloatType, 4, 4>> mat(glm::value_ptr(transform));
  Eigen::Matrix<FloatType, 3, 1> translation(mat.block(0, 3, 3, 1));
  Eigen::Quaternion<FloatType> rotation(Eigen::Matrix<FloatType, 3, 3>(mat.topLeftCorner(3, 3)));
  return Rigid3<FloatType>(translation, rotation);
}

using Rigid3d = Rigid3<double>;
using Rigid3f = Rigid3<float>;

}  // namespace transform
}  // namespace math

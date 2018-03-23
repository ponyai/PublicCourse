// Copyright @2018 Pony AI Inc. All rights reserved.
#include <gtest/gtest.h>

#include "homework3/rotation/rotation.h"

namespace homework3 {
namespace {

// Both functions should not crash even for no rotation.
TEST(Rotation, BasicTest) {
  Eigen::Matrix3d rotation;
  rotation << 1, 0, 0,
              0, 1, 0,
              0, 0, 1;
  auto roll_pitch_yaw = ToRollPitchYaw(rotation);
  auto angle_axis = ToAngleAxis(rotation);
  EXPECT_DOUBLE_EQ(0.0, roll_pitch_yaw.x());
  EXPECT_DOUBLE_EQ(0.0, angle_axis.angle());
}

TEST(Rotation, RollPitchYawTest) {
  Eigen::Matrix3d rotation;
  rotation << 0.97517033, -0.153792, -0.15934508,
              0.0978434, 0.94470249, -0.31299183,
              0.19866933, 0.28962948, 0.93629336;
  auto roll_pitch_yaw = ToRollPitchYaw(rotation);
  EXPECT_NEAR(0.1, roll_pitch_yaw.x(), 1e-3);
  EXPECT_NEAR(-0.2, roll_pitch_yaw.y(), 1e-3);
  EXPECT_NEAR(0.3, roll_pitch_yaw.z(), 1e-3);
}

TEST(Rotation, AngleAxisTest) {
  Eigen::Matrix3d rotation;
  rotation << 0.99934681, -0.03017958, 0.0199023,
              0.02997888, 0.99949734, 0.01032745,
              -0.02020334, -0.00972537, 0.99974821;
  auto angle_axis = ToAngleAxis(rotation);
  EXPECT_NEAR(1.0, angle_axis.axis().norm(), 3e-2);
  EXPECT_NEAR(0.037, std::abs(angle_axis.angle()), 3e-2);
  EXPECT_NEAR(-2 * angle_axis.angle() * angle_axis.axis().x(),
              angle_axis.angle() * angle_axis.axis().y(), 3e-2);
  EXPECT_NEAR(-3 * angle_axis.angle() * angle_axis.axis().x(),
              angle_axis.angle() * angle_axis.axis().z(), 3e-2);
}

}  // namespace
}  // namespace homework3


// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include <Eigen/Core>

struct PointCloud {
  // Lidar points in its local coordinate system.
  std::vector<Eigen::Vector3d> points;
  std::vector<bool> is_in_roi;

  // Rotation and translation to transform LiDAR points to world coordinate system.
  Eigen::Matrix3d rotation = Eigen::Matrix3d::Identity();
  Eigen::Vector3d translation = Eigen::Vector3d::Zero();
};

PointCloud ReadPointCloudFromTextFile(const std::string& file_name, bool with_roi = false);
void WritePointcloudToTextFile(const PointCloud& pointcloud, const std::string& file_name);


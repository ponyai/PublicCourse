// Copyright @2019 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include <Eigen/Core>

#include "common/utils/math/vec2d.h"
#include "homework2/pointcloud.h"
#include "homework3/obstacle.h"

class SingleFrameDetector {
 public:
  SingleFrameDetector();

  void GetGroundAndObstacles(
      const PointCloud& point_cloud,
      std::vector<Eigen::Vector3d>* ground_points,
      std::vector<Obstacle>* obstacles);
};

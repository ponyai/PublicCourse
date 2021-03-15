// Copyright @2019 Pony AI Inc. All rights reserved.

#include "homework3/single_frame_detector.h"

#include "glog/logging.h"

namespace {

// If you want to put all your implementation inside this file, you can add some functions here.
void ASampleFunction() {
  LOG(ERROR) << "Ground points have been detected.";
}

}  // namespace

SingleFrameDetector::SingleFrameDetector() {
  // TODO(you): optional, if you need to do anything to initialize the detector, you can do it here.
  // For example, you can load offline computed ground information here.
}

void SingleFrameDetector::GetGroundAndObstacles(
    const PointCloud& point_cloud,
    std::vector<Eigen::Vector3d>* ground_points,
    std::vector<Obstacle>* obstacles) {
  CHECK(ground_points != nullptr);
  CHECK(obstacles != nullptr);
  // TODO(you): add some code to detect the ground and put all the points insidethe ground_points.
  //  I provide some trivial code here. Please replace them with a better implementation.
  for (const Eigen::Vector3d& point : point_cloud.points) {
    if (point.z() < - 1.3) {
      // Note that the points in point_cloud are represented in a coordinate system where the origin
      // is the position of lidar. However, the output points and polygons should use global
      // corrdinate system. This line shows how to transform between them.
      ground_points->push_back(point_cloud.rotation * point + point_cloud.translation);
    }
  }
  // TODO(you): Run flood fill or other algorithms to get the polygons for the obstacles.
  // Still, I provide a fake implementation, please replace it.
  ASampleFunction();
  obstacles->emplace_back();
  const double x = point_cloud.translation.x();
  const double y = point_cloud.translation.y();
  obstacles->back().polygon.emplace_back(x + 10, y);
  obstacles->back().polygon.emplace_back(x + 9, y);
  obstacles->back().polygon.emplace_back(x + 9, y + 1);
  obstacles->back().floor = point_cloud.translation.z() - 1.3;
  obstacles->back().ceiling = point_cloud.translation.z();
  obstacles->back().id = "Fake obstacle";
}

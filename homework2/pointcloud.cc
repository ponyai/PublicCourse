// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework2/pointcloud.h"

#include <fstream>

#include <glog/logging.h>

PointCloud ReadPointCloudFromTextFile(const std::string& file_name, bool with_roi) {
  FILE* file = std::fopen(file_name.c_str(), "r");
  CHECK(file != nullptr) << "Fail to open file " << file_name;

  PointCloud pointcloud;
  Eigen::Vector3d& translation = pointcloud.translation;
  int num_read = std::fscanf(file, "%lf,%lf,%lf\n",
                             &translation(0), &translation(1), &translation(2));
  CHECK_EQ(3, num_read) << "Fail to read translation from file " << file_name;

  Eigen::Matrix3d& rotation = pointcloud.rotation;
  num_read = std::fscanf(file, "%lf,%lf,%lf\n%lf,%lf,%lf\n%lf,%lf,%lf\n",
                         &rotation(0, 0), &rotation(0, 1), &rotation(0, 2),
                         &rotation(1, 0), &rotation(1, 1), &rotation(1, 2),
                         &rotation(2, 0), &rotation(2, 1), &rotation(2, 2));
  CHECK_EQ(9, num_read) << "Fail to read rotation from file " << file_name;

  Eigen::Vector3d point = Eigen::Vector3d::Zero();
  int point_index = 0;
  char is_in_roi[10] = "";
  if (with_roi) {
    while (std::fscanf(file, "%d,%lf,%lf,%lf,%s\n", &point_index, &point(0),
                       &point(1), &point(2), is_in_roi) != EOF) {
      pointcloud.points.emplace_back(point);
      if (strcmp(is_in_roi, "true") == 0) {
        pointcloud.is_in_roi.push_back(true);
      } else {
        pointcloud.is_in_roi.push_back(false);
      }
    }
  } else {
    while (std::fscanf(file, "%d,%lf,%lf,%lf\n",
                      &point_index, &point(0), &point(1), &point(2)) != EOF) {
      pointcloud.points.emplace_back(point);
    }
  }
  std::fclose(file);

  return pointcloud;
}

void WritePointcloudToTextFile(const PointCloud& pointcloud, const std::string& file_name) {
  std::ofstream fout(file_name);
  const Eigen::Matrix3d& rotation = pointcloud.rotation;
  const Eigen::Vector3d& translation = pointcloud.translation;
  fout << translation.x() << "," << translation.y() << "," << translation.z() << std::endl;
  fout << rotation(0, 0) << "," << rotation(0, 1) << "," << rotation(0, 2) << std::endl;
  fout << rotation(1, 0) << "," << rotation(1, 1) << "," << rotation(1, 2) << std::endl;
  fout << rotation(2, 0) << "," << rotation(2, 1) << "," << rotation(2, 2) << std::endl;

  for (int i = 0; i < pointcloud.points.size(); ++i) {
    const Eigen::Vector3d& pt = pointcloud.points[i];
    fout << i << "," << pt.x() << "," << pt.y() << "," << pt.z() << std::endl;
  }
  fout.close();
}


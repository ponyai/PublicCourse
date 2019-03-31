// Copyright @2019 Pony AI Inc. All rights reserved.

#include <unordered_map>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <QtWidgets/QApplication>

#include "common/utils/math/transform/transform.h"
#include "homework2/icp/icp_viewer.h"

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  // ATTENTION!!! : please use absolute path for reading the data file.
  const PointCloud src_pointcloud = ReadPointCloudFromTextFile("/home/ocean/PublicCourse/homework2/data/src.txt");
  const PointCloud target_pointcloud = ReadPointCloudFromTextFile("/home/ocean/PublicCourse/homework2/data/target.txt");

  QApplication app(argc, argv);
  IcpViewer::Options options;
  IcpViewer viewer(options, nullptr, src_pointcloud, target_pointcloud);
  viewer.resize(1280, 960);
  viewer.show();
  app.exec();

  return 0;
}

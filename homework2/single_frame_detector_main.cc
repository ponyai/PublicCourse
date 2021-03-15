// Copyright @2019 Pony AI Inc. All rights reserved.

#include <unordered_map>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <QtWidgets/QApplication>

#include "homework3/pointcloud_viewer.h"
#include "homework3/single_frame_detector.h"

DEFINE_string(pony_data_dir, "", "The path of pony data.");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  SingleFrameDetector detector;
  QApplication app(argc, argv);
  PointCloudViewer::Options options;
  PointCloudViewer viewer(options, nullptr, FLAGS_pony_data_dir, &detector);
  viewer.resize(1280, 960);
  viewer.show();
  app.exec();

  return 0;
}

// Copyright @2020 Pony AI Inc. All rights reserved.

#include "homework3/cubic_spline.h"

#include <fstream>
#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(input_file_path, "", "Absolute path for input data file");

// TODO: Feel free to modify this file as you need.
int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (FLAGS_input_file_path.empty()) {
    LOG(FATAL) << "Please provide absolute path for your input data file with --input_file_path ";
  }

  std::ifstream data_file(FLAGS_input_file_path);

  std::vector<double> xs;
  std::vector<double> ys;

  double x = 0.0;
  double y = 0.0;
  while (data_file >> x >> y) {
    std::cout << x << " " << y << std::endl;
    xs.push_back(x);
    ys.push_back(y);
  }

  CubicSpline cs;
  cs.SetPoints(xs, ys);
  cs.Interpolate();

  return 0;
}

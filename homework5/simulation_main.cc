// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/display/simulation_main_window.h"

#include <QtWidgets/QApplication>
#include "gflags/gflags.h"
#include "glog/logging.h"

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("pony.ai");
  QCoreApplication::setOrganizationDomain("pony.ai");
  QCoreApplication::setApplicationName("Simulation");

  FLAGS_logtostderr = true;
  display::SimulationMainWindow main_window(nullptr);

  app.installEventFilter(&main_window);
  app.exec();

  return 0;
}

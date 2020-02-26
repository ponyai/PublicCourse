// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/replay_main_window.h"

#include <QtWidgets/QApplication>
#include "common/proto/display.pb.h"

DEFINE_string(simulation_history_path, "simulation_log.bin", "Path of simulation history file");
DEFINE_bool(enable_visualization, true, "True if enable visualization");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("pony.ai");
  QCoreApplication::setOrganizationDomain("pony.ai");
  QCoreApplication::setApplicationName("Replay");

  display::ReplayMainWindow main_window(nullptr, FLAGS_simulation_log_file_path);

  app.installEventFilter(&main_window);
  app.exec();

  return 0;
}

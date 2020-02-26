// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/simulation_main_window.h"

#include <QtWidgets/QApplication>
#include "common/proto/display.pb.h"

DEFINE_string(simulation_config_path, "", "Path of simulation config");
DEFINE_bool(enable_visualization, true, "True if enable visualization");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;

  // If simulation config path is not specified explicitly, use sample config
  if (FLAGS_simulation_config_path.empty()) {
    FLAGS_simulation_config_path =
        file::path::Join(file::path::GetProjectRootPath(), "pnc", "data", "sample.config");
  }
  // Load simulation config
  interface::simulation::SimulationConfig simulation_config;
  LOG(INFO) << "Loading simulation config from " << FLAGS_simulation_config_path;
  CHECK(file::ReadTextFileToProto(FLAGS_simulation_config_path, &simulation_config));

  if (FLAGS_enable_visualization) {
    LOG(INFO) << "Visualization is enabled. Simulation is started with UI window...";

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("pony.ai");
    QCoreApplication::setOrganizationDomain("pony.ai");
    QCoreApplication::setApplicationName("Simulation");
    display::SimulationMainWindow main_window(simulation_config, nullptr);
    app.installEventFilter(&main_window);
    app.exec();
  } else {
    LOG(INFO) << "Visualization is not enabled. Simulation is started without UI window...";
    auto simulation_system = std::make_unique<simulation::SimulationSystem>(simulation_config);
    simulation_system->Initialize();
    simulation_system->Start();
  }
  LOG(INFO) << "Done.";
  return 0;
}

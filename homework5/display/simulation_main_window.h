// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/file/file.h"
#include "homework6/display/main_window.h"
#include "homework6/proto/simulation_config.pb.h"

DEFINE_string(route_file_path, "", "Path of route file");

namespace display {

class SimulationMainWindow : public MainWindow {
 public:
  SimulationMainWindow(QWidget* parent) : MainWindow(parent) {
    setWindowTitle("Simulation");
    interface::homework6::SimulationConfig simulation_config;
    CHECK(file::ReadTextFileToProto(FLAGS_route_file_path, &simulation_config));
    simulation_system_ = std::make_unique<simulation::SimulationSystem>(simulation_config);
    simulation_system_thread_ = std::thread([this]() {
      simulation_system_->Initialize();
      simulation_system_->Start();
    });
    SetupMenu();
  }

  virtual void timerEvent(QTimerEvent* /*event*/) override {
    data_ = simulation_system_->FetchData();
    painter_widget_->set_simulation_system_data(data_);
    painter_widget_->update();
    std::unordered_map<std::string, interface::display::VariableViewList> variable_view_data;
    GetVariableViewList(&variable_view_data, data_);
    variable_dock_widget_->UpdateVariables(variable_view_data);
  }

  void SetupMenu() {
    QAction* action = nullptr;
    QMenu* menu = nullptr;
    // Play
    menu = menuBar()->addMenu(tr("&Play"));

    action = menu->addAction(tr("&Pause/Resume"));
    action->setShortcut(QKeySequence(Qt::Key_Space));
    connect(action, &QAction::triggered, this, [this] { simulation_system_->PushPauseCommand(); });

    menu->addSeparator();

    action = menu->addAction(tr("&Slow Down"));
    QList<QKeySequence> shortcuts;
    shortcuts.clear();
    shortcuts << QKeySequence(Qt::Key_Minus) << QKeySequence(Qt::Key_Underscore);
    action->setShortcuts(shortcuts);
    connect(action, &QAction::triggered, this,
            [this] { simulation_system_->PushSimulationSpeedCommand(0.5); });

    action = menu->addAction(tr("&Speed Up"));
    shortcuts.clear();
    shortcuts << QKeySequence(Qt::Key_Plus) << QKeySequence(Qt::Key_Equal);
    action->setShortcuts(shortcuts);
    connect(action, &QAction::triggered, this,
            [this] { simulation_system_->PushSimulationSpeedCommand(2.0); });

    action = menu->addAction(tr("&Next Iteration"));
    shortcuts.clear();
    shortcuts << QKeySequence(Qt::Key_Right);
    action->setShortcuts(shortcuts);
    connect(action, &QAction::triggered, this,
            [this] { simulation_system_->PushPlaybackNextOneIterationCommand(); });

    menu->addSeparator();
    action = menu->addAction(tr("&Quit"));
    action->setShortcut(QKeySequence::Quit);
    connect(action, &QAction::triggered, this, [this] { QApplication::exit(0); });
  }

 private:
  std::unique_ptr<simulation::SimulationSystem> simulation_system_;
  std::thread simulation_system_thread_;

  interface::simulation::SimulationSystemData data_;
};
};

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/simulation_config.pb.h"
#include "pnc/display/main_window.h"

namespace display {

class SimulationMainWindow : public MainWindow {
 public:
  SimulationMainWindow(const interface::simulation::SimulationConfig& simulation_config,
                       QWidget* parent)
      : MainWindow(simulation_config, parent) {
    simulation_config_.CopyFrom(simulation_config);
    setWindowTitle("Simulation");
    simulation_system_ = std::make_unique<simulation::SimulationSystem>(simulation_config);
    simulation_system_thread_ = std::thread([this]() {
      simulation_system_->Initialize();
      simulation_system_->Start();
    });
    SetupMenu();
  }
  ~SimulationMainWindow() = default;

  void timerEvent(QTimerEvent* /*event*/) override {
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

    if (simulation_config_.add_keyboard_controlled_agent()) {
      action = menu->addAction(tr("Manual Test Agent - Speed Up"));
      shortcuts.clear();
      shortcuts << QKeySequence(Qt::SHIFT + Qt::Key_Up);
      action->setShortcuts(shortcuts);
      connect(action, &QAction::triggered, this,
              [this] { simulation_system_->manual_test_agent()->SpeedUp(); });

      action = menu->addAction(tr("Manual Test Agent - Slow Down"));
      shortcuts.clear();
      shortcuts << QKeySequence(Qt::SHIFT + Qt::Key_Down);
      action->setShortcuts(shortcuts);
      connect(action, &QAction::triggered, this,
              [this] { simulation_system_->manual_test_agent()->SlowDown(); });

      action = menu->addAction(tr("Manual Test Agent - Turn Left"));
      shortcuts.clear();
      shortcuts << QKeySequence(Qt::SHIFT + Qt::Key_Left);
      action->setShortcuts(shortcuts);
      connect(action, &QAction::triggered, this,
              [this] { simulation_system_->manual_test_agent()->TurnLeft(); });

      action = menu->addAction(tr("Manual Test Agent - TurnRight"));
      shortcuts.clear();
      shortcuts << QKeySequence(Qt::SHIFT + Qt::Key_Right);
      action->setShortcuts(shortcuts);
      connect(action, &QAction::triggered, this,
              [this] { simulation_system_->manual_test_agent()->TurnRight(); });

      action = menu->addAction(tr("Manual Test Agent - Stop"));
      shortcuts.clear();
      shortcuts << QKeySequence(Qt::SHIFT + Qt::Key_S);
      action->setShortcuts(shortcuts);
      connect(action, &QAction::triggered, this,
              [this] { simulation_system_->manual_test_agent()->Stop(); });
    }

    MainWindow::SetupMenu();
  }

 private:
  std::unique_ptr<simulation::SimulationSystem> simulation_system_;
  std::thread simulation_system_thread_;

  interface::simulation::SimulationConfig simulation_config_;

  interface::simulation::SimulationSystemData data_;
};
};

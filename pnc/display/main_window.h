// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include "common/proto/simulation_config.pb.h"
#include "pnc/display/interactive_slider_widget.h"
#include "pnc/display/pnc_painter_widget.h"
#include "pnc/display/q_variable_dock_widget.h"
#include "pnc/display/variable_view.h"
#include "pnc/simulation/manual_test_agent.h"
#include "pnc/simulation/simulation_system.h"

namespace display {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(const interface::simulation::SimulationConfig& simulation_config, QWidget* parent)
      : QMainWindow(parent) {
    simulation_config_.CopyFrom(simulation_config);
    auto* container = new QWidget(this);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setMargin(0);
    layout->setSpacing(0);

    utils::display::PncPainterWidget::Options options;
    painter_widget_ = new utils::display::PncPainterWidget(options, container);
    layout->addWidget(painter_widget_);
    setCentralWidget(container);
    resize(1400, 960);
    move(100, 100);
    variable_dock_widget_ = new utils::display::QVariableDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, variable_dock_widget_);
    startTimer(static_cast<int>(1000.0 * 0.1));
    show();
  }

  ~MainWindow() override = default;

 protected:
  void GetVariableViewList(
      std::unordered_map<std::string, interface::display::VariableViewList>* variable_view_data,
      const interface::simulation::SimulationSystemData& simulation_system_data) {
    interface::display::VariableViewList vehicle_status_variable_view_list;
    interface::display::VariableViewList debug_variable_view_list;
    for (const interface::simulation::VehicleAgentData& vehicle_agent_data :
         simulation_system_data.vehicle_agent()) {
      utils::display::VariableView variable_view(vehicle_agent_data.name());
      const interface::agent::AgentStatus& agent_status = vehicle_agent_data.agent_status();
      const interface::agent::VehicleStatus& vehicle_status = agent_status.vehicle_status();
      if (agent_status.simulation_status().is_alive()) {
        variable_view.AddVariable("alive", "true", utils::display::Color::Green());
      } else {
        variable_view.AddVariable("alive", "false", utils::display::Color::Red());
      }
      if (agent_status.simulation_status().is_finished()) {
        variable_view.AddVariable("finished", "true", utils::display::Color::Green());
      } else {
        variable_view.AddVariable("finished", "false", utils::display::Color::Red());
      }
      variable_view.AddIntVariable("num_finished_trips",
                                   agent_status.simulation_status().num_finished_trips());
      variable_view.AddDoubleVariable("simulation_time",
                                      agent_status.simulation_status().simulation_time());
      if (!agent_status.simulation_status().elimination_reason().empty()) {
        variable_view.AddVariable("elimination reason",
                                  agent_status.simulation_status().elimination_reason(),
                                  utils::display::Color::Black(), utils::display::Color::Red());
      }
      variable_view.AddDoubleVariable("position_x", vehicle_status.position().x());
      variable_view.AddDoubleVariable("position_y", vehicle_status.position().y());
      variable_view.AddDoubleVariable("position_z", vehicle_status.position().z());
      variable_view.AddDoubleVariable("orientation_x", vehicle_status.orientation().x());
      variable_view.AddDoubleVariable("orientation_y", vehicle_status.orientation().y());
      variable_view.AddDoubleVariable("orientation_z", vehicle_status.orientation().z());
      variable_view.AddDoubleVariable("orientation_w", vehicle_status.orientation().w());
      variable_view.AddDoubleVariable("velocity_x", vehicle_status.velocity().x());
      variable_view.AddDoubleVariable("velocity_y", vehicle_status.velocity().y());
      variable_view.AddDoubleVariable("velocity_z", vehicle_status.velocity().z());
      variable_view.AddDoubleVariable("angular_velocity_vcs_x",
                                      vehicle_status.angular_velocity_vcs().x());
      variable_view.AddDoubleVariable("angular_velocity_vcs_y",
                                      vehicle_status.angular_velocity_vcs().y());
      variable_view.AddDoubleVariable("angular_velocity_vcs_z",
                                      vehicle_status.angular_velocity_vcs().z());
      variable_view.AddDoubleVariable("acceleration_vcs_x", vehicle_status.acceleration_vcs().x());
      variable_view.AddDoubleVariable("acceleration_vcs_y", vehicle_status.acceleration_vcs().y());
      variable_view.AddDoubleVariable("acceleration_vcs_z", vehicle_status.acceleration_vcs().z());
      vehicle_status_variable_view_list.add_variable_view()->CopyFrom(
          variable_view.ToProtoBuffer());
      const interface::display::VariableView& debug_variable_view =
          vehicle_agent_data.debug_variables();
      debug_variable_view_list.add_variable_view()->CopyFrom(debug_variable_view);
    }
    variable_view_data->emplace("vehicle_status", vehicle_status_variable_view_list);
    variable_view_data->emplace("debug_variables", debug_variable_view_list);
  }

  // TODO refactor setup menu
  void SetupMenu() {
    QAction* action = nullptr;
    QMenu* menu = nullptr;
    // Play
    menu = menuBar()->addMenu(tr("&Perspective"));

    action = menu->addAction(tr("&God"));
    connect(action, &QAction::triggered, this,
            [this] { painter_widget_->SetVehiclePerspective(""); });

    menu->addSeparator();

    for (const auto& agent_config : simulation_config_.agent_config()) {
      const std::string& name = agent_config.name();
      action = menu->addAction(tr(name.data()));
      connect(action, &QAction::triggered, this,
              [this, &name] { painter_widget_->SetVehiclePerspective(name); });
    }
    if (simulation_config_.add_keyboard_controlled_agent()) {
      std::string name = std::string(simulation::kKeyboardControlledAgentName);
      action = menu->addAction(tr(name.data()));
      connect(action, &QAction::triggered, this, [this] {
        painter_widget_->SetVehiclePerspective(
            std::string(simulation::kKeyboardControlledAgentName));
      });
    }
  }

  utils::display::PncPainterWidget* painter_widget_ = nullptr;           // Not owned.
  utils::display::QVariableDockWidget* variable_dock_widget_ = nullptr;  // Not owned.

 private:
  interface::simulation::SimulationConfig simulation_config_;
};

}  // namespace display

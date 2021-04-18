// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include "homework6/display/pnc_painter_widget.h"
#include "homework6/display/q_variable_dock_widget.h"
#include "homework6/display/variable_view.h"
#include "homework6/simulation/simulation_system.h"

namespace display {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent) : QMainWindow(parent) {
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
    for (const interface::simulation::VehicleAgentData& vehicle_agent_data :
         simulation_system_data.vehicle_agent()) {
      utils::display::VariableView variable_view(vehicle_agent_data.name());
      const interface::agent::AgentStatus& agent_status = vehicle_agent_data.agent_status();
      const interface::agent::VehicleStatus& vehicle_status = agent_status.vehicle_status();
      variable_view.AddVariable("alive",
                                agent_status.simulation_status().is_alive() ? "true" : "false");
      variable_view.AddVariable("finished",
                                agent_status.simulation_status().is_finished() ? "true" : "false");
      variable_view.AddDoubleVariable("simulation_time",
                                      agent_status.simulation_status().simulation_time());
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
      *vehicle_status_variable_view_list.add_variable_view() = variable_view.ToProtoBuffer();
    }
    variable_view_data->emplace("vehicle_status", vehicle_status_variable_view_list);
  }

  utils::display::PncPainterWidget* painter_widget_ = nullptr;           // Not owned.
  utils::display::QVariableDockWidget* variable_dock_widget_ = nullptr;  // Not owned.
};

}  // namespace display

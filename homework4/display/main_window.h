// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include "homework4/display/map_lane_dock_widget.h"
#include "homework4/display/map_painter_widget.h"

namespace homework4 {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent) : QMainWindow(parent) {
    map_lib_ = std::make_unique<homework4::map::MapLib>();
    auto* container = new QWidget(this);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setMargin(0);
    layout->setSpacing(0);
    utils::display::MapPainterWidget::Options options;
    painter_widget_ = new utils::display::MapPainterWidget(options, container);
    layout->addWidget(painter_widget_);
    setCentralWidget(container);

    resize(1400, 960);
    move(100, 100);

    startTimer(static_cast<int>(1000.0 * 0.1));

    map_lane_dock_widget_ = new utils::display::MapLaneDockWidget(this);
    map_lane_dock_widget_->Initialize(map_lib_->map_proto());
    addDockWidget(Qt::RightDockWidgetArea, map_lane_dock_widget_);
    show();

    connect(map_lane_dock_widget_, &utils::display::MapLaneDockWidget::HighlightLane,
            painter_widget_, &utils::display::MapPainterWidget::SetHighlightedLane);
  }

  void timerEvent(QTimerEvent* /*event*/) override { painter_widget_->update(); }

  void set_displayed_route(const interface::route::Route& route) {
    painter_widget_->SetDisplayedRoute(route);
  }

  ~MainWindow() override = default;

 private:
  utils::display::MapPainterWidget* painter_widget_ = nullptr;         // Not owned.
  utils::display::MapLaneDockWidget* map_lane_dock_widget_ = nullptr;  // Not owned.

  std::unique_ptr<homework4::map::MapLib> map_lib_;
};

}  // namespace homework4

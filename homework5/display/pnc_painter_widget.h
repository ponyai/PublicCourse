// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QOpenGLWidget>

#include "common/proto/map_lane.pb.h"
#include "common/proto/simulation.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/display/camera.h"
#include "common/utils/display/font_renderer.h"
#include "common/utils/display/layer_manager.h"
#include "common/utils/display/painter_widget_base.h"
#include "common/utils/display/painter_widget_controller.h"
#include "common/utils/display/qt_macro.h"
#include "common/utils/gl/context.h"
#include "homework6/display/layer/intensity_map_layer.h"
#include "homework6/display/layer/road_graph_layer.h"
#include "homework6/display/layer/vehicle_layer.h"
#include "homework6/display/user_interface_data.h"

#include "homework6/display/pnc_gl_painter.h"

namespace utils {
namespace display {

class PncPainterWidget : public PainterWidgetBase {
  Q_OBJECT

 public:
  PncPainterWidget(Options options, QWidget* parent);
  ~PncPainterWidget() = default;

  virtual PncOpenglPainter* gl_painter() override { return gl_painter_.get(); }

  void set_simulation_system_data(const interface::simulation::SimulationSystemData& data) {
    simulation_system_data_.CopyFrom(data);
  }

 protected:
  virtual void Initialize() override;
  virtual void Paint3D() override;
  virtual void InitializeGlPainter() override;
  // void mousePressEvent(QMouseEvent* event);

 private:
  std::unique_ptr<PncOpenglPainter> gl_painter_;

  interface::simulation::SimulationSystemData simulation_system_data_;
  interface::vehicle::VehicleParams vehicle_params_;

  std::unique_ptr<homework6::map::MapLib> map_lib_;
  utils::display::UserInterfaceData user_interface_data_;
};

}  // namespace display
}  // namespace utils

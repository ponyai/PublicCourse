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
#include "pnc/display/layer/intensity_map_layer.h"
#include "pnc/display/layer/obstacle_layer.h"
#include "pnc/display/layer/road_graph_layer.h"
#include "pnc/display/layer/vehicle_layer.h"
#include "pnc/display/user_interface_data.h"

#include "pnc/display/pnc_gl_painter.h"
#include "pnc/display/pnc_painter_widget_controller.h"
#include "pnc/utils/vehicle_status_helper.h"

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

  void SetVehiclePerspective(const std::string& vehicle_name) {
    user_interface_data_.perspective_vehicle = vehicle_name;
  }

 protected:
  void Initialize() override;
  void Paint3D() override;
  void InitializeGlPainter() override;
  void SetupCamera() override;

 private:
  std::unique_ptr<PncOpenglPainter> gl_painter_;

  interface::simulation::SimulationSystemData simulation_system_data_;
  interface::vehicle::VehicleParams vehicle_params_;

  std::unique_ptr<pnc::map::MapLib> map_lib_;
  utils::display::UserInterfaceData user_interface_data_;
};

}  // namespace display
}  // namespace utils

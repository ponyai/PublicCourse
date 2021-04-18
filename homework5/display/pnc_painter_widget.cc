// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/display/pnc_painter_widget.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glog/logging.h>

#include "common/utils/gl/qt/context.h"
#include "common/utils/gl_support/base.h"

#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "homework6/display/pnc_gl_painter.h"
#include "homework6/display/pnc_painter_widget_controller.h"

static constexpr char kLayerRoadGraph[] = "layer/road_graph";
static constexpr char kLayerIntensityMap[] = "layer/intensity_map";
static constexpr char kLayerVehicle[] = "layer/vehicle";
static constexpr int kBackgroundLayerLevel = 1;
static constexpr int kObjectLayerLevel = 3;
static constexpr int kVehicleLayerLevel = 8;

namespace utils {
namespace display {

PncPainterWidget::PncPainterWidget(Options options, QWidget* parent)
    : PainterWidgetBase(options, parent) {
  painter_widget_controller_ =
      std::make_unique<PncPainterWidgetController>(options.screen_coord_system);
  user_interface_data_.camera_controller = painter_widget_controller_->MutableCamera();
  painter_widget_controller_->MutableCamera()->ResetOrientation();
  painter_widget_controller_->MutableCamera()->UpdateCenter(60.0, 60.0, 0.0);
  painter_widget_controller_->MutableCamera()->UpdateDistance(222.0);
}

void PncPainterWidget::Initialize() {
  CHECK(file::ReadFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
  map_lib_ = std::make_unique<homework6::map::MapLib>();
  layer_manager_->AddLayer(
      std::make_unique<utils::display::RoadGraphLayer>(
          kLayerRoadGraph, map_lib_.get(), simulation_system_data_, user_interface_data_),
      kObjectLayerLevel);
  layer_manager_->AddLayer(std::make_unique<utils::display::IntensityMapLayer>(kLayerIntensityMap),
                           kBackgroundLayerLevel);
  layer_manager_->AddLayer(
      std::make_unique<utils::display::VehicleLayer>(kLayerVehicle, simulation_system_data_,
                                                     user_interface_data_, vehicle_params_),
      kVehicleLayerLevel);
}

void PncPainterWidget::InitializeGlPainter() {
  gl_painter_ = std::make_unique<PncOpenglPainter>(gl_context(), font_renderer());
  gl_painter_->SetupOpenGL();
}

void PncPainterWidget::Paint3D() { layer_manager_->DrawLayers(); }

}  // namespace display
}  // namespace utils

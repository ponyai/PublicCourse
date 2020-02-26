// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/pnc_painter_widget.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glog/logging.h>
#include "pnc/utils/vehicle_status_helper.h"

#include "common/utils/gl/qt/context.h"
#include "common/utils/gl_support/base.h"

#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "pnc/display/pnc_gl_painter.h"
#include "pnc/display/pnc_painter_widget_controller.h"

static constexpr char kLayerRoadGraph[] = "layer/road_graph";
static constexpr char kLayerIntensityMap[] = "layer/intensity_map";
static constexpr char kLayerVehicle[] = "layer/vehicle";
static constexpr char kLayerObstacle[] = "layer/obstacle";
static constexpr int kBackgroundLayerLevel = 1;
static constexpr int kObjectLayerLevel = 3;
static constexpr int kVehicleLayerLevel = 8;
static constexpr int kUILayerLevel = 10;

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
  static_cast<PncPainterWidgetController*>(painter_widget_controller_.get())
      ->set_delta_azimuth_angle(0.0f);
}

void PncPainterWidget::Initialize() {
  CHECK(file::ReadFileToProto(utils::path::GetVehicleParamsPath(), &vehicle_params_));
  map_lib_ = std::make_unique<pnc::map::MapLib>();
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
  layer_manager_->AddLayer(std::make_unique<utils::display::ObstacleLayer>(
                               kLayerObstacle, simulation_system_data_, user_interface_data_),
                           kObjectLayerLevel);
}

void PncPainterWidget::InitializeGlPainter() {
  gl_painter_ = std::make_unique<PncOpenglPainter>(gl_context(), font_renderer());
  gl_painter_->SetupOpenGL();
}

void PncPainterWidget::Paint3D() { layer_manager_->DrawLayers(); }

void PncPainterWidget::SetupCamera() {
  if (!user_interface_data_.perspective_vehicle.empty()) {
    double yaw = 0.0;
    double pos_x = 0.0;
    double pos_y = 0.0;
    for (const auto& vehicle_agent_data : simulation_system_data_.vehicle_agent()) {
      if (vehicle_agent_data.name() == user_interface_data_.perspective_vehicle) {
        utils::vehicle::VehicleStatusHelper vehicle_status_helper(
            vehicle_agent_data.agent_status().vehicle_status());
        yaw = math::transform::GetYaw(vehicle_status_helper.orientation());
        pos_x = vehicle_status_helper.position().x();
        pos_y = vehicle_status_helper.position().y();
        break;
      }
    }
    double azimuth_angle =
        static_cast<PncPainterWidgetController*>(painter_widget_controller_.get())
            ->delta_azimuth_angle() +
        yaw + M_PI;
    CameraController* camera = painter_widget_controller_->MutableCamera();

    // Ratio between distance_from_adc_to_center and camera distance is fixed, so that zooming
    // in/out would not result in change of screen position of vehicle.
    const double distance_from_adc_to_center = 0.2 * camera->distance();
    camera->UpdateEye(camera->altitude_angle(), azimuth_angle, camera->distance());
    camera->UpdateCenter(pos_x + distance_from_adc_to_center * std::cos(yaw),
                         pos_y + distance_from_adc_to_center * std::sin(yaw), 0.0);
  }
  painter_widget_controller_->MutableCamera()->UpdateCamera();
}

}  // namespace display
}  // namespace utils

// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/map_painter_widget.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glog/logging.h>

#include "common/utils/gl/qt/context.h"
#include "common/utils/gl_support/base.h"

#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "homework4/display/map_gl_painter.h"

static constexpr char kLayerRoadGraph[] = "layer/road_graph";
static constexpr char kLayerLane[] = "layer/lane";
static constexpr char kLayerIntensityMap[] = "layer/intensity_map";
static constexpr char kLayerRoute[] = "layer/route";
static constexpr int kBackgroundLayerLevel = 1;
static constexpr int kObjectLayerLevel = 3;

namespace utils {
namespace display {

MapPainterWidget::MapPainterWidget(Options options, QWidget* parent)
    : PainterWidgetBase(options, parent) {
  user_interface_data_.camera_controller = painter_widget_controller_->MutableCamera();
  painter_widget_controller_->MutableCamera()->ResetOrientation();
  painter_widget_controller_->MutableCamera()->UpdateCenter(60.0, 60.0, 0.0);
}

void MapPainterWidget::Initialize() {
  map_lib_ = std::make_unique<homework4::map::MapLib>();
  layer_manager_->AddLayer(std::make_unique<utils::display::RoadGraphLayer>(
                               kLayerRoadGraph, map_lib_.get()),
                           kObjectLayerLevel);
  layer_manager_->AddLayer(std::make_unique<utils::display::IntensityMapLayer>(kLayerIntensityMap),
                           kBackgroundLayerLevel);
  layer_manager_->AddLayer(
      std::make_unique<utils::display::LaneLayer>(kLayerLane, highlighted_lanes_),
      kObjectLayerLevel);
  layer_manager_->AddLayer(
      std::make_unique<utils::display::RouteLayer>(kLayerRoute, route_, user_interface_data_),
      kObjectLayerLevel);
}

void MapPainterWidget::InitializeGlPainter() {
  gl_painter_ = std::make_unique<MapOpenglPainter>(gl_context(), font_renderer());
  gl_painter_->SetupOpenGL();
}

void MapPainterWidget::Paint3D() { layer_manager_->DrawLayers(); }

void MapPainterWidget::SetHighlightedLane(std::string lane_id) {
  highlighted_lanes_.clear();
  for (const interface::map::Lane& lane : map_lib_->map_proto().lane()) {
    if (lane_id == lane.id().id()) {
      highlighted_lanes_.push_back(lane);
      break;
    }
  }
}

void MapPainterWidget::SetDisplayedRoute(const interface::route::Route& route) {
  route_.CopyFrom(route);
}

}  // namespace display
}  // namespace utils

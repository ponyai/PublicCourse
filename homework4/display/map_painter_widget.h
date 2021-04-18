// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QOpenGLWidget>

#include "common/proto/map_lane.pb.h"
#include "common/utils/display/camera.h"
#include "common/utils/display/font_renderer.h"
#include "common/utils/display/layer_manager.h"
#include "common/utils/display/painter_widget_base.h"
#include "common/utils/display/painter_widget_controller.h"
#include "common/utils/display/qt_macro.h"
#include "common/utils/gl/context.h"
#include "homework4/display/layer/intensity_map_layer.h"
#include "homework4/display/layer/lane_layer.h"
#include "homework4/display/layer/road_graph_layer.h"
#include "homework4/display/layer/route_layer.h"
#include "homework4/display/map_gl_painter.h"
#include "homework4/display/user_interface_data.h"

namespace utils {
namespace display {

class MapPainterWidget : public PainterWidgetBase {
  Q_OBJECT

 public:
  MapPainterWidget(Options options, QWidget* parent);
  ~MapPainterWidget() = default;

  virtual MapOpenglPainter* gl_painter() override { return gl_painter_.get(); }

  virtual void Initialize() override;
  virtual void Paint3D() override;
  virtual void InitializeGlPainter() override;

  void SetHighlightedLane(std::string lane_id);
  void SetDisplayedRoute(const interface::route::Route& route);

 private:
  std::unique_ptr<MapOpenglPainter> gl_painter_;
  std::unique_ptr<homework4::map::MapLib> map_lib_;

  utils::display::UserInterfaceData user_interface_data_;
  std::vector<interface::map::Lane> highlighted_lanes_;
  interface::route::Route route_;
};

}  // namespace display
}  // namespace utils

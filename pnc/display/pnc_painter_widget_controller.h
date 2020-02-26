// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/display/painter_widget_controller.h"
#include "common/utils/math/vec2d.h"

#include "common/utils/display/rendering_controllers.h"

#include "glog/logging.h"

namespace utils {
namespace display {

class PncPainterWidgetController : public PainterWidgetController {
 public:
  explicit PncPainterWidgetController(ScreenCoordSystem screen_coord_sys)
      : PainterWidgetController(screen_coord_sys) {}
  ~PncPainterWidgetController() override = default;

  void MousePressEvent(QMouseEvent* event) override;
  double delta_azimuth_angle() const { return delta_azimuth_angle_; }
  void set_delta_azimuth_angle(double angle) { delta_azimuth_angle_ = angle; }

 private:
  double delta_azimuth_angle_ = 0.0;
};

};  // namespace display
}  // namespace utils

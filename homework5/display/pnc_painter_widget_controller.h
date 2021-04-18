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
  ~PncPainterWidgetController() = default;

  virtual void MousePressEvent(QMouseEvent* event) override;
};

};  // namespace display
}  // namespace utils

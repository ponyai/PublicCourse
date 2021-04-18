// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc_painter_widget_controller.h"

namespace utils {
namespace display {

void PncPainterWidgetController::MousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton &&
      (event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) {
    // Only a left-click of mouse could trigger this branch.
    QPoint pos = event->pos();
    math::Vec2d xy_coord = camera_->PickPointOnXYPlane(math::Vec2d(pos.x(), pos.y()));
    LOG(ERROR) << "Clicked point: x:" << xy_coord.x << ",   y:" << xy_coord.y;
  } else {
    utils::display::PainterWidgetController::MousePressEvent(event);
  }
}
}
}  // namespace utils

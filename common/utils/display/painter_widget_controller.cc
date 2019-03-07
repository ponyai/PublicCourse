// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/painter_widget_controller.h"

namespace utils {
namespace display {

namespace {
constexpr const char kCameraDistance[] = "display/camera_distance";
}  // namespace

PainterWidgetController::PainterWidgetController(ScreenCoordSystem screen_coord_sys)
    : camera_(std::make_unique<CameraController>(screen_coord_sys)),
      mouse_(std::make_unique<MouseController>()),
      view_(std::make_unique<ViewController>(camera_.get())) {
  QSettings qsettings;
  if (qsettings.contains(kCameraDistance)) {
    camera_->UpdateEye(camera_->altitude_angle(), camera_->azimuth_angle(),
                       qsettings.value(kCameraDistance).toDouble());
  }
}

void PainterWidgetController::KeyPressEvent(QKeyEvent* /*event*/) {}
void PainterWidgetController::KeyReleaseEvent(QKeyEvent* /*event*/) {}

void PainterWidgetController::WheelEvent(QWheelEvent* event) {
  if (event->delta() > 0) {
    camera_->ZoomIn();
  } else {
    camera_->ZoomOut();
  }
  QSettings qsettings;
  qsettings.setValue(kCameraDistance, camera_->distance());
}

void PainterWidgetController::MousePressEvent(QMouseEvent* event) {
  camera_->MarkAsReference();
  mouse_->UpdateMouseLocation(event->pos().x(), event->pos().y());

  if (event->button() == Qt::LeftButton) {
    mouse_->UpdateStatus(MouseStatus::kRotate);
  } else if (event->button() == Qt::RightButton) {
    mouse_->UpdateStatus(MouseStatus::kMove);
  }
}

void PainterWidgetController::MouseDoubleClickEvent(QMouseEvent* event) {
  camera_->MarkAsReference();
  mouse_->UpdateMouseLocation(event->pos().x(), event->pos().y());
}

void PainterWidgetController::MouseMoveEvent(QMouseEvent* event) {
  QPoint pos = event->pos();
  double dx = pos.x() - mouse_->mouse_location().pos_x;
  double dy = pos.y() - mouse_->mouse_location().pos_y;
  mouse_->UpdateMouseLocation(event->pos().x(), event->pos().y());
  if (mouse_->status() == MouseStatus::kRotate) {
    camera_->RotateAroundReference(dx * (pos.y() > camera_->widget_height() / 2 ? -1 : 1), dy);
  } else if (mouse_->status() == MouseStatus::kMove) {
    camera_->Panning(dx, dy);
  }
  camera_->MarkAsReference();
}

void PainterWidgetController::MouseReleaseEvent(QMouseEvent* /*event*/) {
  mouse_->UpdateStatus(MouseStatus::kNone);
}

}  // namespace display
}  // namespace utils

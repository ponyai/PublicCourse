// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <map>
#include <memory>

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QGLWidget>

#include "common/utils/display/rendering_controllers.h"

namespace utils {
namespace display {

class PainterWidgetController {
 public:
  explicit PainterWidgetController(ScreenCoordSystem screen_coord_sys);
  virtual ~PainterWidgetController() = default;
  // Event Interfaces.
  virtual void KeyPressEvent(QKeyEvent* event);
  virtual void KeyReleaseEvent(QKeyEvent* event);
  virtual void WheelEvent(QWheelEvent* event);
  virtual void MousePressEvent(QMouseEvent* event);
  virtual void MouseDoubleClickEvent(QMouseEvent* event);
  virtual void MouseMoveEvent(QMouseEvent* event);
  virtual void MouseReleaseEvent(QMouseEvent* event);
  // Access Interfaces.
  CameraController* MutableCamera() { return camera_.get(); }
  const CameraController& GetCamera() const { return *camera_; }
  MouseController* MutableMouse() { return mouse_.get(); }
  const MouseController& GetMouse() const { return *mouse_; }
  ViewController* MutableView() { return view_.get(); }
  const ViewController& GetView() const { return *view_; }

 protected:
  std::unique_ptr<CameraController> camera_;
  std::unique_ptr<MouseController> mouse_;
  std::unique_ptr<ViewController> view_;
};

}  // namespace display
}  // namespace utils

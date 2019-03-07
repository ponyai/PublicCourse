// Copyright @2018 Pony AI Inc. All rights reserved.
#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QOpenGLWidget>

#include "common/utils/display/camera.h"
#include "common/utils/display/font_renderer.h"
#include "common/utils/display/layer_manager.h"
#include "common/utils/display/painter_widget_controller.h"
#include "common/utils/display/qt_macro.h"
#include "common/utils/gl/context.h"

namespace utils {
namespace display {

class PainterWidgetBase : public QOpenGLWidget {
  Q_OBJECT

 public:
  struct Options {
    ScreenCoordSystem screen_coord_system = ScreenCoordSystem::OpenGl;
    bool set_auto_fill_background = false;
    bool set_mouse_tracking = false;
  };

  PainterWidgetBase(Options options, QWidget* parent);
  virtual ~PainterWidgetBase() ;

  virtual FontRenderer* font_renderer() { return font_renderer_.get(); }

  gl::Context* gl_context() const { return CHECK_NOTNULL(gl_context_.get()); }

 protected:
  virtual void Initialize() = 0;
  virtual void InitializeGlPainter() = 0;
  virtual void FetchData(){};
  virtual void Paint3D();
  virtual void SetupCamera();
  virtual OpenglPainter* gl_painter() = 0;

  std::unique_ptr<FontRenderer> font_renderer_;
  std::unique_ptr<LayerManager> layer_manager_;
  std::unique_ptr<PainterWidgetController> painter_widget_controller_;

 private:
  // Qt event handlers.
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int new_width, int new_height) final;

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) final;
  void mousePressEvent(QMouseEvent* event) final;
  void mouseMoveEvent(QMouseEvent* event) final;
  void mouseDoubleClickEvent(QMouseEvent* event) final;
  void mouseReleaseEvent(QMouseEvent* event) final;

 private:
  std::unique_ptr<gl::Context> gl_context_;
  Options options_;
};

}  // namespace display
}  // namespace utils

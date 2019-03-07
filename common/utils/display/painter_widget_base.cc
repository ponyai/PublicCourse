// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/painter_widget_base.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glog/logging.h>

#include "common/utils/gl/qt/context.h"
#include "common/utils/gl_support/base.h"

#include "common/utils/file/file.h"
#include "common/utils/file/path.h"

namespace utils {
namespace display {

PainterWidgetBase::PainterWidgetBase(Options options, QWidget* parent)
    : QOpenGLWidget(parent), options_(options) {
  painter_widget_controller_ =
      std::make_unique<PainterWidgetController>(options_.screen_coord_system);
  CHECK_NOTNULL(painter_widget_controller_.get());
  setMouseTracking(options.set_mouse_tracking);
  setAutoFillBackground(options.set_auto_fill_background);
}

PainterWidgetBase::~PainterWidgetBase() {}

void PainterWidgetBase::initializeGL() {
  gl_context_ = std::make_unique<gl::qt::Context>(context(), context()->surface());
  gl_context_->MakeCurrent();

  font_renderer_ = std::make_unique<FontRenderer>(this);

  InitializeGlPainter();
  // Construct the layer_manager here
  layer_manager_ =
      std::make_unique<LayerManager>(gl_context_.get(), gl_painter(), font_renderer_.get());
  // Call the derived initialization functions.
  Initialize();
  gl_context_->DoneCurrent();
}

void PainterWidgetBase::resizeGL(int new_width, int new_height) {
  glViewport(0, 0, new_width, new_height);
  gl_painter()->OnTargetResize(new_width, new_height);
  auto* camera = painter_widget_controller_->MutableCamera();
  camera->ResizeWidget(new_width, new_height);
  layer_manager_->Resize(new_width, new_height);
}

void PainterWidgetBase::SetupCamera() {
  painter_widget_controller_->MutableCamera()->UpdateCamera();
}

void PainterWidgetBase::Paint3D() { layer_manager_->DrawLayers(); }

void PainterWidgetBase::paintGL() {
  gl_context_->MakeCurrent();
  FetchData();

  // Set screen/framebuffer related values.
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set rendering states.
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glPointSize(1.0f);

  // Set the matrices.
  SetupCamera();
  const auto& camera = painter_widget_controller_->GetCamera();
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(camera.view_matrix()));

  Paint3D();

  glShadeModel(GL_FLAT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  font_renderer_->Flush();
  gl_context_->DoneCurrent();
}

void PainterWidgetBase::keyPressEvent(QKeyEvent* event) {
  painter_widget_controller_->KeyPressEvent(event);
}

void PainterWidgetBase::keyReleaseEvent(QKeyEvent* event) {
  painter_widget_controller_->KeyReleaseEvent(event);
}

void PainterWidgetBase::wheelEvent(QWheelEvent* event) {
  painter_widget_controller_->WheelEvent(event);
}

void PainterWidgetBase::mousePressEvent(QMouseEvent* event) {
  painter_widget_controller_->MousePressEvent(event);
}

void PainterWidgetBase::mouseDoubleClickEvent(QMouseEvent* event) {
  painter_widget_controller_->MouseDoubleClickEvent(event);
}

void PainterWidgetBase::mouseMoveEvent(QMouseEvent* event) {
  painter_widget_controller_->MouseMoveEvent(event);
}

void PainterWidgetBase::mouseReleaseEvent(QMouseEvent* event) {
  painter_widget_controller_->MouseReleaseEvent(event);
}

}  // namespace display
}  // namespace utils

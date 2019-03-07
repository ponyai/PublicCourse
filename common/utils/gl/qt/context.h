// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions_3_3_Compatibility>

#include "common/utils/gl/context.h"
#include "common/utils/gl/functions.h"
#include "glog/logging.h"

namespace gl {
namespace qt {

class Context : public ::gl::Context {
 public:
  // Makes an offscreen opengl context.
  Context();

  // Makes an opengl context with specified qt opengl context and surface, e.g. from an OpenGL
  // window or widget.
  Context(QOpenGLContext* context, QSurface* surface);

  ~Context() override = default;

  void MakeCurrent() override;
  void DoneCurrent() override;
  void SwapBuffers() override;

  FuncPointer GetProcAddress(const char* proc_name) const override;

 private:
  // In case an offscreen context and surface are created by this class itself, then they are owned
  // by the class instance. If the context and surface are from an existing widget or window, then
  // they are managed by Qt.
  QOpenGLContext* context_ = nullptr;
  QSurface* surface_ = nullptr;

  std::unique_ptr<QOpenGLContext> offscreen_context_;
  std::unique_ptr<QOffscreenSurface> offscreen_surface_;
  std::unique_ptr<QOpenGLFunctions_3_3_Compatibility> functions_;
};

// ======================================== Implementation ========================================

inline Context::Context(QOpenGLContext* context, QSurface* surface)
    : context_(CHECK_NOTNULL(context)), surface_(CHECK_NOTNULL(surface)) {
  Initialize();
}

inline Context::Context() {
  offscreen_surface_ = std::make_unique<QOffscreenSurface>();
  offscreen_surface_->create();
  surface_ = offscreen_surface_.get();

  offscreen_context_ = std::make_unique<QOpenGLContext>();
  offscreen_context_->setFormat(offscreen_surface_->requestedFormat());
  offscreen_context_->create();
  context_ = offscreen_context_.get();

  Initialize();
}

inline void Context::MakeCurrent() { context_->makeCurrent(surface_); }

inline void Context::DoneCurrent() { context_->doneCurrent(); }

inline void Context::SwapBuffers() { context_->swapBuffers(surface_); }

inline Context::FuncPointer Context::GetProcAddress(const char* proc_name) const {
  return context_->getProcAddress(proc_name);
}

}  // namespace qt
}  // namespace gl

// Copyright @2018 Pony AI Inc. All rights reserved.

#include <string>

#include "common/utils/display/font_renderer.h"
#include "common/utils/display/gl_painter.h"
#include "common/utils/gl/context.h"

#pragma once

namespace utils {
namespace display {

// A layer is a basic abstraction to draw different things over the OpenGL painter widget.
class Layer {
 public:
  explicit Layer(const std::string& name) : name_(name) {}
  virtual ~Layer() = default;

  void InitializeContext(gl::Context* context, OpenglPainter* gl_painter,
                         FontRenderer* font_renderer) {
    context_ = CHECK_NOTNULL(context);
    gl_painter_ = CHECK_NOTNULL(gl_painter);
    font_renderer_ = CHECK_NOTNULL(font_renderer);
    InitializeOnContextCreated();
  }

  int width() const { return width_; }

  int height() const { return height_; }

  void Enable() { enabled_ = true; }

  void Disable() { enabled_ = false; }

  bool IsEnabled() const { return enabled_; }

  virtual void Resize(int width, int height) {
    width_ = width;
    height_ = height;
  }

  std::string GetName() const { return name_; }

  virtual void Draw() const = 0;

 protected:
  // This function will be called right after the context is created.
  virtual void InitializeOnContextCreated() {}
  int target_width() const { return width_; }
  int target_height() const { return height_; }

  std::string name_ = "";
  gl::Context* context_ = nullptr;         // Not owned.
  OpenglPainter* gl_painter_ = nullptr;    // Not owned.
  FontRenderer* font_renderer_ = nullptr;  // Not owned.
  int width_ = 0;
  int height_ = 0;
  bool enabled_ = false;

  DISALLOW_COPY_MOVE_AND_ASSIGN(Layer);
};

}  // namespace display
}  // namespace utils

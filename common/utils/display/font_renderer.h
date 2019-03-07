// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <QtWidgets/QOpenGLWidget>

#include "common/utils/common/optional.h"
#include "common/utils/display/color.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

namespace utils {
namespace display {

// A temporary font renderer for migrating from QGLWidget to QOpenGLWidget. It will be replaced by a
// testable and more portable font renderer that does not depend on Qt in the future.
//
// QPainter is used to draw texts over a QOpenGLWidget. Since QPainter changes a lot of global
// OpenGL attributes, it is required to enclose the font rendering by glPushAttrib() and
// glPopAttrib(), which is not cheap. To resolve this issue, we just accumulate all font draw calls,
// and do not execute them until Flush() is called.
class FontRenderer {
 public:
  enum class BackgroundDrawMode {
    kNone = 0,
    kBordered = 1,
    kFilled = 2,
  };

  struct RenderTask {
    std::string text;
    math::Vec2d screen_pos;
    Color color;
    std::string font_name = "Helvetica";
    int font_size = 14;
    BackgroundDrawMode background_mode = BackgroundDrawMode::kNone;
  };

  explicit FontRenderer(QOpenGLWidget* widget) : widget_(CHECK_NOTNULL(widget)) {}
  virtual ~FontRenderer() = default;

  // Draws texts at specified 2D screen position with default font.
  void DrawText2D(const std::string& text, const math::Vec2d& screen_pos, Color color,
                  BackgroundDrawMode mode = BackgroundDrawMode::kNone);

  // Draws texts at specified 2D screen position with specified font.
  void DrawText2D(const std::string& text, const math::Vec2d& screen_pos, Color color,
                  const std::string& font_name, int font_size,
                  BackgroundDrawMode mode = BackgroundDrawMode::kNone);

  // Draws texts at specified 3D position with default font.
  void DrawText3D(const std::string& text, const math::Vec3d& point, Color color);

  // Draws texts at specified 3D position with specified font.
  void DrawText3D(const std::string& text, const math::Vec3d& point, Color color,
                  const std::string& font_name, int font_size);

  // Executes and clear all pending tasks.
  void Flush();

 private:
  utils::Optional<math::Vec2d> Project(const math::Vec3d& point) const;

  QOpenGLWidget* widget_ = nullptr;  // Not owned.
  std::vector<RenderTask> render_tasks_;
};

}  // namespace display
}  // namespace utils

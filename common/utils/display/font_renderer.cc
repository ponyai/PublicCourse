// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/font_renderer.h"

#include <GL/glu.h>
#include <QtGui/QPainter>

namespace utils {
namespace display {

void FontRenderer::DrawText2D(const std::string& text, const math::Vec2d& screen_pos, Color color,
                              BackgroundDrawMode mode) {
  render_tasks_.emplace_back();
  RenderTask& task = render_tasks_.back();
  task.text = text;
  task.screen_pos = screen_pos;
  task.color = color;
  task.background_mode = mode;
}

void FontRenderer::DrawText2D(const std::string& text, const math::Vec2d& screen_pos, Color color,
                              const std::string& font_name, int font_size,
                              BackgroundDrawMode mode) {
  render_tasks_.emplace_back();
  RenderTask& task = render_tasks_.back();
  task.text = text;
  task.screen_pos = screen_pos;
  task.color = color;
  task.font_name = font_name;
  task.font_size = font_size;
  task.background_mode = mode;
}

void FontRenderer::DrawText3D(const std::string& text, const math::Vec3d& point, Color color) {
  auto p = Project(point);
  if (p) {
    DrawText2D(text, *p, color);
  }
}

void FontRenderer::DrawText3D(const std::string& text, const math::Vec3d& point, Color color,
                              const std::string& font_name, int font_size) {
  auto p = Project(point);
  if (p) {
    DrawText2D(text, *p, color, font_name, font_size);
  }
}

void FontRenderer::Flush() {
  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_MULTISAMPLE_BIT);
  QPainter painter(widget_);
  for (const RenderTask& task : render_tasks_) {
    QFont font(QString::fromStdString(task.font_name), task.font_size);
    if (task.background_mode != BackgroundDrawMode::kNone) {
      QFontMetrics font_metrics(font);
      const int pixels_width = font_metrics.width(task.text.c_str());
      const int pixels_height = font_metrics.height();
      painter.setPen(QColor(255, 255, 255, 166));
      QRect rect(task.screen_pos.x - 1, task.screen_pos.y - pixels_height + 5, pixels_width + 2,
                 pixels_height);
      if (task.background_mode == BackgroundDrawMode::kFilled) {
        painter.fillRect(rect, Qt::white);
      } else {
        painter.drawRect(rect);
      }
    }
    painter.setPen(QColor::fromRgbF(task.color.red, task.color.green, task.color.blue));
    painter.setFont(font);
    painter.drawText(QPointF(task.screen_pos.x, task.screen_pos.y),
                     QString::fromStdString(task.text));
  }
  render_tasks_.clear();
  glPopAttrib();
}

utils::Optional<math::Vec2d> FontRenderer::Project(const math::Vec3d& point) const {
  GLdouble model[16], proj[16];
  GLint view[4];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetIntegerv(GL_VIEWPORT, view);

  GLdouble screen_x, screen_y, screen_z;
  gluProject(point.x, point.y, point.z, model, proj, view, &screen_x, &screen_y, &screen_z);
  // Valid screen_z should be within [0.0, 1.0].
  if (screen_z < 0.0 || screen_z > 1.0) {
    return utils::None();
  }
  return math::Vec2d(screen_x, widget_->height() - screen_y);
}

}  // namespace display
}  // namespace utils

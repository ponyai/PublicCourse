// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/gl_painter.h"

#include <glog/logging.h>
#include <QtWidgets/QOpenGLWidget>

#include "common/proto/geometry.pb.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/basic_primitives.h"

namespace utils {
namespace display {

OpenglPainter::OpenglPainter(::gl::Context* gl_context, FontRenderer* font_renderer)
    : gl_context_(CHECK_NOTNULL(gl_context)), font_renderer_(font_renderer) {
  primitive_painter_ = std::make_unique<PrimitivePainter>(gl_context_);
}

void OpenglPainter::SetupOpenGL() {
  glEnable(GL_MULTISAMPLE);
  ASSERT_GL_OK();

  float light_ambient[] = {0, 0, 0, 0};
  float light_diffuse[] = {1, 1, 1, 1};
  float light_specular[] = {1, 1, 1, 1};
  float light_position[] = {0, 0, 100, 0};

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
}

void OpenglPainter::Begin2D() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, target_width(), 0.0, target_height());

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
}

void OpenglPainter::End2D() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glEnable(GL_DEPTH_TEST);
}

template <>
void OpenglPainter::DrawArrow<math::Vec3d>(const math::Vec3d& start, const math::Vec3d& end,
                                           const ArrowStyle<math::Vec3d>& style) const {
  if (start.DistanceToPoint(end) < math::kEpsilon) {
    return;
  }

  // up_dir should be perpendicular to the center line.
  const math::Vec3d arrow_dir = start - end;
  CHECK_LT(std::abs<double>((arrow_dir).InnerProd(style.up_dir)), math::kEpsilon);

  const double w = std::cos(math::DegreeToRadian(style.wing_angle / 2.0));
  const math::Vec3d dir = style.up_dir * std::sin(math::DegreeToRadian(style.wing_angle / 2.0));
  const Eigen::Matrix3d r1 = Eigen::Quaterniond(w, dir.x, dir.y, dir.z).toRotationMatrix();
  const Eigen::Matrix3d r2 = Eigen::Quaterniond(-w, dir.x, dir.y, dir.z).toRotationMatrix();
  const math::Vec3d wing1(r1 * math::ToMatrix(arrow_dir));
  const math::Vec3d wing2(r2 * math::ToMatrix(arrow_dir));

  constexpr double kLengthScale = 0.3;
  utils::FixedArray<math::Vec3d> point_buffer(6);
  point_buffer[0] = start;
  point_buffer[1] = end;
  point_buffer[2] = end;
  point_buffer[3] = end + wing1 * kLengthScale;
  point_buffer[4] = end;
  point_buffer[5] = end + wing2 * kLengthScale;
  DrawLines(utils::ConstArrayView<math::Vec3d>(point_buffer.data(), point_buffer.size()), GL_LINES,
            style.line_style);
}

template <typename T>
void OpenglPainter::DrawAxis(const glm::tmat4x4<T>& axises,
                             const AxisStyle<math::Vec3<T>>& style) const {
  glm::tvec3<T> origin_point = axises[3];
  if (style.show_origin_point) {
    primitive_painter_->DrawPoints(utils::ConstArrayView<glm::tvec3<T>>(&origin_point, 1),
                                   style.origin_point_style);
  }
  glm::tvec3<T> x_axis = axises * glm::tvec4<T>(style.x_axis_length, 0.0, 0.0, 1.0);
  std::vector<glm::tvec3<T>> x_axis_ps = {origin_point, x_axis};
  DrawLines(utils::ConstArrayView<glm::tvec3<T>>(x_axis_ps.data(), x_axis_ps.size()), GL_LINES,
            style.x_axis_style);

  glm::tvec3<T> y_axis = axises * glm::tvec4<T>(0.0, style.x_axis_length, 0.0, 1.0);
  std::vector<glm::tvec3<T>> y_axis_ps = {origin_point, y_axis};
  DrawLines(utils::ConstArrayView<glm::tvec3<T>>(y_axis_ps.data(), y_axis_ps.size()), GL_LINES,
            style.y_axis_style);

  glm::tvec3<T> z_axis = axises * glm::tvec4<T>(0.0, 0.0, style.x_axis_length, 1.0);
  std::vector<glm::tvec3<T>> z_axis_ps = {origin_point, z_axis};
  DrawLines(utils::ConstArrayView<glm::tvec3<T>>(z_axis_ps.data(), z_axis_ps.size()), GL_LINES,
            style.z_axis_style);
}

}  // namespace display
}  // namespace utils

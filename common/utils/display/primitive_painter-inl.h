// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

namespace utils {
namespace display {

template <typename T>
void PrimitivePainter::DrawPoints(const utils::ConstArrayView<T>& points,
                                  const PointStyle& style) const {
  const auto& gl = context_->functions();
  static_assert(math::Dimension<T>::kValue == 1 || math::Dimension<T>::kValue == 3,
                "Points should be draw in 1 or 3 dim vector.");
  static_assert(std::is_floating_point<typename math::Dimension<T>::ElemType>::value,
                "Points coord should be double/float.");

  gl.PushAttrib(GL_POINT_BIT | GL_CURRENT_BIT);
  gl.PointSize(style.point_size);
  gl.Color4d(style.point_color.red, style.point_color.green, style.point_color.blue, 1.0);

  gl::VertexBuffer buffer(context_);
  CHECK(buffer.SetData(points));

  constexpr GLenum float_type =
      std::is_same<typename math::Dimension<T>::ElemType, float>::value ? GL_FLOAT : GL_DOUBLE;
  gl.EnableClientState(GL_VERTEX_ARRAY);
  {
    gl::BindingGuard guard(&buffer);
    gl.VertexPointer(3, float_type, 0, nullptr);
  }
  gl.DrawArrays(GL_POINTS, 0, points.size() * math::Dimension<T>::kValue / 3);
  gl.DisableClientState(GL_VERTEX_ARRAY);
  gl.PopAttrib();
}

template <typename PointT>
void PrimitivePainter::DrawLines(const utils::ConstArrayView<PointT>& points, GLenum mode,
                                 const LineStyle& style) const {
  static_assert(std::is_floating_point<typename math::Dimension<PointT>::ElemType>::value,
                "Line coord should be double/float.");
  static_assert(math::Dimension<PointT>::kValue == 2 || math::Dimension<PointT>::kValue == 3,
                "Points should be draw in 2 or 3 dim vector.");
  const auto& gl = context_->functions();
  const auto dim = GetDimension<PointT>();
  const auto float_type = ToGLFloatType<PointT>();

  gl.PushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
  gl.LineWidth(style.line_width);
  if (style.is_dash_line) {
    gl.LineStipple(1, 0x00FF);
    gl.Enable(GL_LINE_STIPPLE);
  }

  gl::VertexBuffer vertex_buffer(context_);
  CHECK(vertex_buffer.SetData(points));

  std::vector<glm::vec3> colors(points.size());
  for (int i = 0; i < points.size(); ++i) {
    colors[i].r = style.line_color.red;
    colors[i].g = style.line_color.green;
    colors[i].b = style.line_color.blue;
  }

  gl::VertexBuffer color_buffer(context_);
  CHECK(color_buffer.SetData(utils::ConstArrayView<glm::vec3>(colors.data(), colors.size())));

  gl.EnableClientState(GL_VERTEX_ARRAY);
  gl.EnableClientState(GL_COLOR_ARRAY);
  {
    gl::BindingGuard guard(&vertex_buffer);
    gl.VertexPointer(dim, float_type, 0, nullptr);
  }
  {
    gl::BindingGuard guard(&color_buffer);
    gl.ColorPointer(3, GL_FLOAT, 0, nullptr);
  }
  gl.Color3d(style.line_color.red, style.line_color.green, style.line_color.blue);
  gl.DrawArrays(mode, 0, points.size());

  gl.DisableClientState(GL_VERTEX_ARRAY);
  gl.DisableClientState(GL_COLOR_ARRAY);
  gl.PopAttrib();
}

template <typename PointT>
void PrimitivePainter::DrawStippleLines(const utils::ConstArrayView<PointT>& points, GLenum mode,
                                        const StippleLineStyle& style) const {
  CHECK(style.stipple_factor != -1 && style.stipple_pattern != -1);
  const auto& gl = context_->functions();
  gl.PushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
  gl.Enable(GL_LINE_STIPPLE);
  gl.LineStipple(style.stipple_factor, style.stipple_pattern);
  DrawLines<PointT>(points, mode, style.line_style);
  gl.PopAttrib();
}

template <typename PointT>
void PrimitivePainter::DrawPlane(const utils::ConstArrayView<PointT>& points,
                                 const PlaneStyle& style) const {
  static_assert(3 == math::Dimension<PointT>::kValue, "Object surface should be in 3 dim space.");
  static_assert(std::is_floating_point<typename math::Dimension<PointT>::ElemType>::value,
                "Object surface coordinate should be double/float.");
  // Set color
  const auto& gl = context_->functions();
  gl.PushAttrib(GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
  gl.Disable(GL_CULL_FACE);
  gl.Disable(GL_DEPTH_TEST);
  gl.Enable(GL_BLEND);
  gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.Color4d(style.plane_color.red, style.plane_color.green, style.plane_color.blue, style.alpha);

  // Surface may be a concave polygon, while opengl can only draw the convex ones. So we need
  // to triangulate the non-convex polygon into some small convex ones.
  GLUtesselator* tesslator = CHECK_NOTNULL(gluNewTess());
  gluTessCallback(tesslator, GLU_TESS_BEGIN, (void (*)())glBegin);
  gluTessCallback(tesslator, GLU_TESS_VERTEX, (void (*)())glVertex3dv);
  gluTessCallback(tesslator, GLU_TESS_END, glEnd);

  gluTessBeginPolygon(tesslator, 0);
  gluTessBeginContour(tesslator);
  for (auto iter = points.begin(); iter != points.end(); iter++) {
    gluTessVertex(tesslator, reinterpret_cast<typename math::Dimension<PointT>::ElemType*>(
                                 const_cast<PointT*>(&(*iter))),
                  reinterpret_cast<typename math::Dimension<PointT>::ElemType*>(
                      const_cast<PointT*>(&(*iter))));
  }
  gluTessEndContour(tesslator);
  gluTessEndPolygon(tesslator);
  gluDeleteTess(tesslator);

  gl.PopAttrib();
}

template <typename PointT>
void PrimitivePainter::DrawSurface(const utils::ConstArrayView<PointT>& points,
                                   const SurfaceStyle& style) const {
  if (style.show_vertices) {
    DrawPoints(points, style.vertice_style);
  }
  if (style.show_edge) {
    DrawLines(points, GL_LINE_LOOP, style.edge_style);
  }
  if (style.show_plane) {
    DrawPlane(points, style.plane_style);
  }
}

template <typename Point2DT>
void PrimitivePainter::DrawPrism(const utils::ConstArrayView<Point2DT>& points, double ceiling,
                                 double floor, const PrismStyle& style) const {
  utils::FixedArray<math::Vec3d> horizontal_surface(points.size());
  // Draw ceiling.
  for (int i = 0; i < points.size(); ++i) {
    horizontal_surface[i] = math::Vec3d(points[i], ceiling);
  }
  DrawSurface(
      utils::ConstArrayView<math::Vec3d>(horizontal_surface.data(), horizontal_surface.size()),
      style);
  // Draw vertical surfaces.
  utils::FixedArray<math::Vec3d> vertical_surface(4);
  for (int i = 0; i < static_cast<int64_t>(points.size()); ++i) {
    vertical_surface[0] = math::Vec3d(points[i], floor);
    vertical_surface[1] = math::Vec3d(points[(i + 1) % points.size()], floor);
    vertical_surface[2] = math::Vec3d(points[(i + 1) % points.size()], ceiling);
    vertical_surface[3] = math::Vec3d(points[i], ceiling);
    DrawSurface(
        utils::ConstArrayView<math::Vec3d>(vertical_surface.data(), vertical_surface.size()),
        style);
  }
  // Draw floor.
  for (int i = 0; i < points.size(); ++i) {
    horizontal_surface[i] = math::Vec3d(points[i], floor);
  }
  DrawSurface(
      utils::ConstArrayView<math::Vec3d>(horizontal_surface.data(), horizontal_surface.size()),
      style);
}
}  // namespace display
}  // namespace utils

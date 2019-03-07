// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/basic_primitives.h"
#include "common/utils/gl/functions.h"
#include "common/utils/gl/geometry_utils.h"

namespace gl {

Points::Points(const Context* context) : Primitive(context) {}

void Points::DrawData() const { DrawData(context_, rendering_state_, vertices_count_); }

void Points::DrawData(const Context* context, const RenderingState& rendering_state,
                      int vertices_count) {
  const auto& gl = context->functions();
  gl.PushAttrib(GL_POINT_BIT | GL_CURRENT_BIT);
  gl.PointSize(rendering_state.point_size);
  gl.DrawArrays(GL_POINTS, 0, vertices_count);
  gl.PopAttrib();
}

Lines::Lines(const Context* context, DrawMode draw_mode)
    : Primitive(context), draw_mode_(draw_mode) {}

void Lines::SetDrawMode(DrawMode draw_mode) { draw_mode_ = draw_mode; }

void Lines::DrawData() const { DrawData(context_, draw_mode_, rendering_state_, vertices_count_); }

void Lines::DrawData(const Context* context, DrawMode draw_mode,
                     const RenderingState& rendering_state, int vertices_count) {
  const auto& gl = context->functions();
  gl.PushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
  gl.LineWidth(rendering_state.line_width);
  if (rendering_state.line_dashed) {
    gl.LineStipple(1, 0x00FF);
    gl.Enable(GL_LINE_STIPPLE);
  } else if (rendering_state.line_stipple) {
    gl.LineStipple(rendering_state.line_stipple_factor, rendering_state.line_stipple_pattern);
    gl.Enable(GL_LINE_STIPPLE);
  }
  gl.DrawArrays(static_cast<GLuint>(draw_mode), 0, vertices_count);
  gl.PopAttrib();
}

Triangles::Triangles(const Context* context, DrawMode draw_mode)
    : Primitive(context), draw_mode_(draw_mode) {}

void Triangles::SetDrawMode(DrawMode draw_mode) { draw_mode_ = draw_mode; }

void Triangles::DrawData() const {
  DrawData(context_, draw_mode_, rendering_state_, vertices_count_);
}

void Triangles::DrawData(const Context* context, DrawMode draw_mode,
                         const RenderingState& /* rendering_state */, int vertices_count) {
  const auto& gl = context->functions();
  gl.DrawArrays(static_cast<GLuint>(draw_mode), 0, vertices_count);
}

Quads::Quads(const Context* context) : Primitive(context) {}

void Quads::DrawData() const {
  const auto& gl = context_->functions();
  for (int i = 0; i < vertices_count_; i += 4) {
    gl.DrawArrays(GL_TRIANGLE_FAN, i, 4);
  }
}

Polygon::Polygon(const Context* context) : Primitive(context) {}

void Polygon::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                      const utils::ConstArrayView<math::Vec2d>& texcoords) {
  ResetAllArrayBuffers();
  std::vector<int> triangle_indices;
  Triangulate(vertices, &triangle_indices);
  auto new_vertices = geometry_utils::ReconstructArrayByIndex(vertices, triangle_indices);
  auto new_texcoords = geometry_utils::ReconstructArrayByIndex(texcoords, triangle_indices);
  vertices_count_ = new_vertices.size();
  CHECK(vertices_buffer_->SetData(
      utils::ConstArrayView<math::Vec3d>(new_vertices.data(), new_vertices.size())));
  CHECK(texcoords_buffer_->SetData(
      utils::ConstArrayView<math::Vec2d>(new_texcoords.data(), new_texcoords.size())));
}

void Polygon::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                      const utils::ConstArrayView<::utils::display::Color>& colors) {
  ResetAllArrayBuffers();
  std::vector<int> triangle_indices;
  Triangulate(vertices, &triangle_indices);
  auto new_vertices = geometry_utils::ReconstructArrayByIndex(vertices, triangle_indices);
  auto new_colors = geometry_utils::ReconstructArrayByIndex(colors, triangle_indices);
  vertices_count_ = new_vertices.size();
  CHECK(vertices_buffer_->SetData(
      utils::ConstArrayView<math::Vec3d>(new_vertices.data(), new_vertices.size())));
  CHECK(colors_buffer_->SetData(
      utils::ConstArrayView<::utils::display::Color>(new_colors.data(), new_colors.size())));
}

void Polygon::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                      const utils::ConstArrayView<math::Vec2d>& texcoords,
                      const utils::ConstArrayView<::utils::display::Color>& colors) {
  ResetAllArrayBuffers();
  std::vector<int> triangle_indices;
  Triangulate(vertices, &triangle_indices);
  auto new_vertices = geometry_utils::ReconstructArrayByIndex(vertices, triangle_indices);
  auto new_texcoords = geometry_utils::ReconstructArrayByIndex(texcoords, triangle_indices);
  auto new_colors = geometry_utils::ReconstructArrayByIndex(colors, triangle_indices);
  vertices_count_ = new_vertices.size();
  CHECK(vertices_buffer_->SetData(
      utils::ConstArrayView<math::Vec3d>(new_vertices.data(), new_vertices.size())));
  CHECK(texcoords_buffer_->SetData(
      utils::ConstArrayView<math::Vec2d>(new_texcoords.data(), new_texcoords.size())));
  CHECK(colors_buffer_->SetData(
      utils::ConstArrayView<::utils::display::Color>(new_colors.data(), new_colors.size())));
}

void Polygon::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                      const ::utils::display::Color& color) {
  std::vector<::utils::display::Color> colors(vertices.size(), color);
  SetData(vertices, utils::ConstArrayView<::utils::display::Color>(colors.data(), colors.size()));
}

void Polygon::DrawData() const {
  const auto& gl = context_->functions();
  gl.DrawArrays(GL_TRIANGLES, 0, vertices_count_);
}

void Polygon::Triangulate(const utils::ConstArrayView<math::Vec3d>& vertices,
                          std::vector<int>* triangle_indices) {
  triangle_indices->clear();
  if (vertices.size() < 3) {
    for (int i = 0; vertices.size(); ++i) {
      triangle_indices->push_back(i);
    }
  } else {
    geometry_utils::EarClippingTriangulation(vertices, triangle_indices);
  }
}

}  // namespace gl

// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/map_gl_painter.h"

#include <QtWidgets/QOpenGLWidget>
#include <glog/logging.h>

#include "common/proto/geometry.pb.h"
#include "common/proto/map_lane.pb.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/basic_primitives.h"
#include "homework4/display/gl_layers.h"

using interface::geometry::Point3D;
using math::Vec2d;
using math::Vec3d;
using utils::ConstArrayView;
using utils::gl_support::Material;
using utils::gl_support::MeshModel;

namespace utils {
namespace display {

namespace {

struct LineWidth {
  static constexpr float kHighlight = 3.0f;
  static constexpr float kRoadGraph = 1.0f;
  static constexpr float kRoadGraphBold = 2.0f;
  static constexpr float kRoute = 12.0f;

  inline static float zoom_ratio(float distance_to_camera) {
    constexpr float kBaseCameraDistance = 30.0f;
    return kBaseCameraDistance / distance_to_camera;
  }

  inline static float route(float distance_to_camera) {
    return std::max(kRoute * zoom_ratio(distance_to_camera), 1.0f);
  }
};

constexpr float LineWidth::kRoadGraph;
constexpr float LineWidth::kRoute;

template <class T>
Vec2d ToVec2d(const T& point) {
  return Vec2d(point.x(), point.y());
}
}  // namespace

MapOpenglPainter::MapOpenglPainter(::gl::Context* gl_context, FontRenderer* font_renderer)
    : OpenglPainter(gl_context, font_renderer) {}

GLuint MapOpenglPainter::LoadImage(const std::string& image_name) {
  QImage image;
  image.load(QString::fromStdString(image_name));

  GLuint texture;
  // Create texture object
  glGenTextures(1, &texture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (image.isGrayscale()) {
    image = image.mirrored().convertToFormat(QImage::Format_Grayscale8);
    if (image.isNull()) {
      LOG(ERROR) << "failed to transform image to opengl format.";
      return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, image.width(), image.height(), 0, GL_RED,
                 GL_UNSIGNED_BYTE, image.constBits());

    GLint swizzle_mask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
  } else {
    image = image.mirrored().convertToFormat(QImage::Format_RGB32);
    if (image.isNull()) {
      LOG(ERROR) << "failed to transform image to opengl format.";
      return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image.constBits());
  }
  return texture;
}

void MapOpenglPainter::DrawRoadGraph(const interface::map::Map& road_graph,
                                     bool enable_lane_highlight,
                                     bool draw_lane_line_realistically) {
  // Road
  std::vector<Vec3d> line_vertices;
  for (const auto& lane : road_graph.lane()) {
    // Center line
    Color center_line_color;
    float center_line_width;
    const interface::geometry::Polyline& center_line = lane.central_line();
    SetLaneCenterLineDrawStyle(enable_lane_highlight, lane, &center_line_width, &center_line_color);
    line_vertices.clear();
    for (const auto& point : center_line.point()) {
      line_vertices.push_back(Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kLayerRoadGraph)));
    }
    primitive_painter_->DrawLines(ConstArrayView<Vec3d>(line_vertices.data(), line_vertices.size()),
                                  GL_LINES, LineStyle(center_line_color, center_line_width, false));

    Color left_bound_color;
    float left_bound_line_width;
    GLushort left_bound_line_stipple_style = 0xffff;
    const interface::map::LaneBoundary& left_bound = lane.left_bound();
    SetLaneBoundaryDrawStyle(enable_lane_highlight, draw_lane_line_realistically, lane, left_bound,
                             &left_bound_line_width, &left_bound_line_stipple_style,
                             &left_bound_color);
    line_vertices.clear();
    for (const auto& point : left_bound.boundary().point()) {
      line_vertices.push_back(Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kLayerRoadGraph)));
    }
    primitive_painter_->DrawStippleLines(
        ConstArrayView<Vec3d>(line_vertices.data(), line_vertices.size()), GL_LINE_STRIP,
        StippleLineStyle(left_bound_color, left_bound_line_width, 1,
                         left_bound_line_stipple_style));

    Color right_bound_color;
    float right_bound_line_width;
    GLushort right_bound_line_stipple_style = 0xffff;
    const interface::map::LaneBoundary& right_bound = lane.right_bound();
    SetLaneBoundaryDrawStyle(enable_lane_highlight, draw_lane_line_realistically, lane, right_bound,
                             &right_bound_line_width, &right_bound_line_stipple_style,
                             &right_bound_color);
    line_vertices.clear();
    for (const auto& point : right_bound.boundary().point()) {
      line_vertices.push_back(Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kLayerRoadGraph)));
    }
    primitive_painter_->DrawStippleLines(
        ConstArrayView<Vec3d>(line_vertices.data(), line_vertices.size()), GL_LINE_STRIP,
        StippleLineStyle(right_bound_color, right_bound_line_width, 1,
                         right_bound_line_stipple_style));
  }
}

void MapOpenglPainter::DrawHighlightedLine(const interface::geometry::Polyline& line,
                                           float line_width, Color line_color) {
  std::vector<Vec3d> line_vertices;
  for (const auto& point : line.point()) {
    line_vertices.push_back(Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kHighlightedLines)));
  }
  primitive_painter_->DrawLines(ConstArrayView<Vec3d>(line_vertices.data(), line_vertices.size()),
                                GL_LINES, LineStyle(line_color, line_width, false));
}

void MapOpenglPainter::DrawHighlightedLanes(const std::vector<interface::map::Lane>& lanes,
                                            Color color) {
  for (const auto& lane : lanes) {
    DrawHighlightedLine(lane.central_line(), LineWidth::kRoadGraph * 3, color);
    DrawHighlightedLine(lane.left_bound().boundary(), LineWidth::kRoadGraph * 3, color);
    DrawHighlightedLine(lane.right_bound().boundary(), LineWidth::kRoadGraph * 3, color);
  }
}

void MapOpenglPainter::SetLaneCenterLineDrawStyle(bool /* enable_lane_highlight */,
                                                  const interface::map::Lane& /* lane */,
                                                  float* line_width, Color* color) {
  *line_width = LineWidth::kRoadGraph;
  *color = Color::White();
}

void MapOpenglPainter::SetLaneBoundaryDrawStyle(bool enable_lane_highlight,
                                                bool draw_lane_line_realistically,
                                                const interface::map::Lane& /*lane*/,
                                                const interface::map::LaneBoundary& boundary,
                                                float* line_width, GLushort* line_stipple_style,
                                                Color* color) {
  *line_width = LineWidth::kRoadGraph;
  *color = Color::White();

  if (enable_lane_highlight) {
    if (boundary.type() == interface::map::LaneBoundary::CURB) {
      *line_width = LineWidth::kHighlight;
      *color = Color::Green();
    }
  }
  if (draw_lane_line_realistically) {
    *line_width = LineWidth::kRoadGraphBold;
    if (boundary.type() == interface::map::LaneBoundary::SOLID_WHITE) {
      *line_stipple_style = 0xffff;
      *color = Color::White();
    } else if (boundary.type() == interface::map::LaneBoundary::SOLID_YELLOW) {
      *line_stipple_style = 0xffff;
      *color = Color::Yellow();
    } else if (boundary.type() == interface::map::LaneBoundary::DOTTED_WHITE) {
      *line_stipple_style = 0x00ff;
      *color = Color::White();
    } else if (boundary.type() == interface::map::LaneBoundary::DOTTED_YELLOW) {
      *line_stipple_style = 0x00ff;
      *color = Color::Yellow();
    } else if (boundary.type() == interface::map::LaneBoundary::CURB) {
      *line_stipple_style = 0xffff;
      *color = Color::White();
    } else {
      LOG(ERROR) << "Unknown lane boundary type " +
                        interface::map::LaneBoundary_Type_Name(boundary.type());
      *line_stipple_style = 0xffff;
      *color = Color::White();
    }
  }
}

void MapOpenglPainter::DrawVerticalSign(glm::vec3 center, glm::vec2 size, float angle,
                                        const std::string& texture_name) {
  using gl::Context;
  using gl::Texture;
  TexturePool::iterator it = textures_.find(texture_name);
  if (it == textures_.end()) {
    std::string texture_path = file::path::Join(file::path::GetProjectRootPath(), "homework4",
                                                "data", "textures", texture_name);
    it = textures_.emplace(texture_name, Texture::CreateFromFile(texture_path, gl_context_)).first;
    it->second->GenerateMipmap();
  }

  const Context::Functions& f = gl_context_->functions();

  std::vector<math::Vec2d> texcoords = {
      Vec2d(0.0, 1.0), Vec2d(1.0, 1.0), Vec2d(1.0, 0.0), Vec2d(0.0, 0.0),
  };

  Texture& texture = *it->second;

  std::vector<math::Vec3d> vertices(4);
  f.Color3f(1.0f, 1.0f, 1.0f);
  f.Enable(GL_TEXTURE_2D);
  f.TexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  f.Enable(GL_BLEND);
  f.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  f.Enable(GL_ALPHA_TEST);
  f.AlphaFunc(GL_GEQUAL, 0.1f);
  texture.Bind();
  float dx = size.x * std::cos(angle);
  float dy = size.x * std::sin(angle);
  vertices[0] = Vec3d(center.x - dx, center.y - dy, center.z - size.y);
  vertices[1] = Vec3d(center.x + dx, center.y + dy, center.z - size.y);
  vertices[2] = Vec3d(center.x + dx, center.y + dy, center.z + size.y);
  vertices[3] = Vec3d(center.x - dx, center.y - dy, center.z + size.y);
  primitive_painter_->DrawQuadsWithTexcoord(
      utils::ConstArrayView<Vec3d>(vertices.data(), vertices.size()),
      utils::ConstArrayView<Vec2d>(texcoords.data(), texcoords.size()));
  texture.Unbind();
  f.Disable(GL_ALPHA_TEST);
  f.Disable(GL_BLEND);
  f.Disable(GL_TEXTURE_2D);
}

void MapOpenglPainter::DrawFlag(const interface::geometry::Point2D& point,
                                const std::string& flag_filename,
                                const utils::display::UserInterfaceData& user_interface_data) {
  constexpr float kOffset = 0.31640625f;  // center flag pole (127-46)/256
  float flag_size = 0.75f + user_interface_data.camera_controller->distance() * 0.025f;
  flag_size = std::min(std::max(flag_size, 0.75f), 32.0f);
  float angle = user_interface_data.camera_controller->azimuth_angle() + M_PI * 0.5;
  float offset = kOffset * flag_size * 2.0f;
  float dx = offset * std::cos(angle);
  float dy = offset * std::sin(angle);
  glm::vec2 size(flag_size, flag_size);
  glm::vec3 center(point.x() + dx, point.y() + dy, flag_size);
  DrawVerticalSign(center, size, angle, flag_filename);
}

void MapOpenglPainter::DrawRoute(const interface::route::Route& route,
                                 const utils::display::UserInterfaceData& user_interface_data) {
  if (route.has_start_point()) {
    const interface::geometry::Point2D& start_point = route.start_point();
    DrawFlag(start_point, "flag_green.png", user_interface_data);
  }
  if (route.has_end_point()) {
    const interface::geometry::Point2D& end_point = route.end_point();
    DrawFlag(end_point, "flag_red.png", user_interface_data);
  }
  if (route.route_point_size() > 0) {
    std::vector<math::Vec3d> line_vertices;
    for (int i = 1; i < route.route_point_size(); i++) {
      const auto& last_point = route.route_point(i - 1);
      const auto& current_point = route.route_point(i);
      line_vertices.push_back(math::Vec3d(last_point.x(), last_point.y(), GetGlLayer(kLayerRoute)));
      line_vertices.push_back(
          math::Vec3d(current_point.x(), current_point.y(), GetGlLayer(kLayerRoute)));
    }
    primitive_painter_->DrawLines(
        ConstArrayView<math::Vec3d>(line_vertices.data(), line_vertices.size()), GL_LINES,
        OpenglPainter::LineStyle(Color::Green(), 3.0f, false));
  }
}

}  // namespace display
}  // namespace utils

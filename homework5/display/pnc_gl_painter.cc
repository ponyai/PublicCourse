// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/display/pnc_gl_painter.h"

#include <glog/logging.h>
#include <QtWidgets/QOpenGLWidget>

#include "common/proto/geometry.pb.h"
#include "common/proto/map_lane.pb.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/basic_primitives.h"
#include "homework6/display/gl_layers.h"

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

void Vehicle::SetDiffuseColor(const utils::display::Color& color) {
  Material& material = body.mutable_materials()->at(2);
  material.diffuse[0] = color.red;
  material.diffuse[1] = color.green;
  material.diffuse[2] = color.blue;
}

void Vehicle::Draw() const {
  glPushMatrix();
  glScalef(5.0, 5.0, 5.0);
  body.Draw();
  glPopMatrix();

  // Velodyne laser
  glPushMatrix();
  glTranslated(-0.34, 0, 1.9);
  glScalef(0.4, 0.4, 0.4);
  velodyne.Draw();
  glPopMatrix();
}

PncOpenglPainter::PncOpenglPainter(::gl::Context* gl_context, FontRenderer* font_renderer)
    : OpenglPainter(gl_context, font_renderer) {
  std::string model_dir = utils::path::Get3dModelsDirectory();
  traffic_light_model_.LoadFromFile(file::path::Join(model_dir, "traffic_light.obj"));
  LoadVehicle();
}

void PncOpenglPainter::LoadVehicle() {
  std::string model_dir = utils::path::Get3dModelsDirectory();
  vehicle_.body.LoadFromFile(file::path::Join(model_dir, "passat.obj"));
  vehicle_.velodyne.LoadFromFile(file::path::Join(model_dir, "velodyne.obj"));
  vehicle_.SetDiffuseColor(Color::White());
}

GLuint PncOpenglPainter::LoadImage(const std::string& image_name) {
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

void PncOpenglPainter::DrawRoadGraph(const interface::map::Map& road_graph,
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
void PncOpenglPainter::SetLaneCenterLineDrawStyle(bool /* enable_lane_highlight */,
                                                  const interface::map::Lane& /* lane */,
                                                  float* line_width, Color* color) {
  *line_width = LineWidth::kRoadGraph;
  *color = Color::White();
}

void PncOpenglPainter::SetLaneBoundaryDrawStyle(bool enable_lane_highlight,
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

void PncOpenglPainter::DrawVerticalSign(glm::vec3 center, glm::vec2 size, float angle,
                                        const std::string& texture_name) {
  using gl::Context;
  using gl::Texture;
  TexturePool::iterator it = textures_.find(texture_name);
  if (it == textures_.end()) {
    std::string texture_path = file::path::Join(file::path::GetProjectRootPath(), "common", "data",
                                                "textures", texture_name);
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

}  // namespace display
}  // namespace utils

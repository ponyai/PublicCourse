// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>

#include <GL/gl.h>
#include <QtWidgets/QOpenGLWidget>
#include <glm/glm.hpp>

#include "common/proto/map.pb.h"
#include "common/proto/route.pb.h"
#include "common/utils/display/color.h"
#include "common/utils/display/font_renderer.h"
#include "common/utils/display/gl_painter.h"
#include "common/utils/display/primitive_painter.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/texture.h"
#include "common/utils/gl_support/base.h"
#include "common/utils/gl_support/mesh_model.h"
#include "common/utils/math/transform/rigid_transform.h"
#include "common/utils/math/transform/transform.h"
#include "common/utils/math/vec2d.h"
#include "homework4/display/user_interface_data.h"
#include "homework4/map/map_lib.h"

namespace utils {
namespace display {

struct IntensityMapImage {
  GLuint image_id;
  interface::map::MapImageConfig map_image_config;
};

class MapOpenglPainter : public OpenglPainter {
 public:
  MapOpenglPainter(gl::Context* context, FontRenderer* font_renderer);
  virtual ~MapOpenglPainter() = default;

  void DrawRoadGraph(const interface::map::Map& road_graph, bool enable_lane_highlight,
                     bool draw_lane_line_realistically);

  void DrawHighlightedLanes(const std::vector<interface::map::Lane>& lanes, Color color);

  void DrawRoute(const interface::route::Route& route,
                 const utils::display::UserInterfaceData& user_interface_data);

  GLuint LoadImage(const std::string& image_name);

  void DrawVerticalSign(glm::vec3 center, glm::vec2 size, float angle,
                        const std::string& texture_name);

 private:
  void SetLaneCenterLineDrawStyle(bool enable_lane_highlight, const interface::map::Lane& lane,
                                  float* line_width, Color* color);

  void SetLaneBoundaryDrawStyle(bool enable_lane_highlight, bool draw_lane_line_realistically,
                                const interface::map::Lane& lane,
                                const interface::map::LaneBoundary& boundary, float* line_width,
                                GLushort* line_stipple_style, Color* color);

  void DrawHighlightedLine(const interface::geometry::Polyline& line, float width, Color color);

  void DrawFlag(const interface::geometry::Point2D& point, const std::string& flag_filename,
                const utils::display::UserInterfaceData& user_interface_data);

  typedef std::map<std::string, std::unique_ptr<gl::Texture>> TexturePool;
  TexturePool textures_;
};

}  // namespace display
}  // namespace utils

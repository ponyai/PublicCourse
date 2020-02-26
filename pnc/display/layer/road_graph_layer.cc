// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/layer/road_graph_layer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/utils/containers/utils.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/base.h"
#include "common/utils/gl/basic_primitives.h"
#include "gflags/gflags.h"
#include "pnc/display/gl_layers.h"
#include "pnc/display/layer/layers_common.h"
#include "pnc/display/pnc_gl_painter.h"

namespace utils {
namespace display {

using interface::geometry::Point3D;

namespace {

struct LineWidth {
  static constexpr float kStopLineWithTrafficLight = 2.0f;
  static constexpr float kTrafficLightConnector = 1.0f;
};

constexpr double kDefaultTrafficLightHeight = 3.0;

Point3D GetMiddlePoint(const google::protobuf::RepeatedPtrField<Point3D>& points) {
  const int n = points.size();
  CHECK_GT(n, 0);
  double length = 0;
  std::vector<double> segment_length(n - 1);
  for (int i = 0; i + 1 < n; ++i) {
    const Point3D& p1 = points.Get(i);
    const Point3D& p2 = points.Get(i + 1);
    segment_length[i] = std::hypot(p1.x() - p2.x(), p1.y() - p2.y());
    length += segment_length[i];
  }
  double s = length / 2.0;
  for (int i = 0; i + 1 < n; ++i) {
    double d = segment_length[i];
    if (d >= s) {
      const Point3D& p1 = points.Get(i);
      const Point3D& p2 = points.Get(i + 1);
      Point3D point;
      double ratio = (d <= math::kEpsilon ? 0.5 : s / d);
      point.set_x(math::Lerp(p1.x(), p2.x(), ratio));
      point.set_y(math::Lerp(p1.y(), p2.y(), ratio));
      point.set_z(math::Lerp(p1.z(), p2.z(), ratio));
      return point;
    }
    s -= d;
  }
  return points.Get(n - 1);
}

}  // namespace anonymous

RoadGraphLayer::RoadGraphLayer(
    const std::string& name, const pnc::map::MapLib* map_lib,
    const interface::simulation::SimulationSystemData& simulation_world_data,
    const UserInterfaceData& user_interface_data)
    : RoadGraphLayerBase(name, map_lib),
      simulation_world_data_(simulation_world_data),
      user_interface_data_(user_interface_data) {
  std::string model_dir = utils::path::Get3dModelsDirectory();
  traffic_light_model_.LoadFromFile(file::path::Join(model_dir, "traffic_light.obj"));
}

void RoadGraphLayer::Draw() const {
  static_cast<PncOpenglPainter*>(gl_painter_)->DrawRoadGraph(map_data(), true, true);
  DrawTrafficLightModel(map_data(), simulation_world_data_);
}

void RoadGraphLayer::DrawTrafficLightModel(
    const interface::map::Map& road_graph,
    const interface::simulation::SimulationSystemData& simulation_world_data) const {
  std::unordered_map<std::string, Color> color_map;
  if (simulation_world_data.has_traffic_light_status()) {
    for (const auto& traffic_light_status :
         simulation_world_data.traffic_light_status().single_traffic_light_status()) {
      if (traffic_light_status.color() == interface::map::Bulb::RED) {
        color_map[traffic_light_status.id().id()] = Color::Red();
      }
      if (traffic_light_status.color() == interface::map::Bulb::GREEN) {
        color_map[traffic_light_status.id().id()] = Color::Green();
      }
      if (traffic_light_status.color() == interface::map::Bulb::YELLOW) {
        color_map[traffic_light_status.id().id()] = Color::Yellow();
      }
    }
  }

  gl::Lines stop_lines(context_);
  stop_lines.GetMutableRenderingState()->line_width = LineWidth::kStopLineWithTrafficLight;

  gl::Lines traffic_lines(context_);
  traffic_lines.GetMutableRenderingState()->line_width = LineWidth::kTrafficLightConnector;
  traffic_lines.GetMutableRenderingState()->line_stipple = true;
  traffic_lines.GetMutableRenderingState()->line_stipple_factor = 1;
  traffic_lines.GetMutableRenderingState()->line_stipple_pattern = 0x3333;

  std::vector<math::Vec3d> line_vertices;
  for (const auto& traffic_light : road_graph.traffic_light()) {
    if (traffic_light.bulb().empty()) {
      continue;
    }
    Point3D traffic_light_point = traffic_light.bulb(1).location();
    traffic_light_point.set_z(kDefaultTrafficLightHeight);

    line_vertices.clear();
    math::Vec2d orientation;
    const auto& stop_line = traffic_light.stop_line();
    CHECK_GE(stop_line.point_size(), 2);
    for (const auto& point : stop_line.point()) {
      line_vertices.push_back(
          math::Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kLayerRoadGraphRoi)));
    }
    // Draw stop line.
    Color stop_line_color = Color::Blue();
    if (color_map.find(traffic_light.id().id()) != color_map.end()) {
      stop_line_color = color_map[traffic_light.id().id()];
    }
    stop_lines.SetData(
        utils::ConstArrayView<math::Vec3d>(line_vertices.data(), line_vertices.size()),
        stop_line_color);
    stop_lines.Draw();
    ASSERT_GL_OK();

    // Hack: always use the first and last point of the stop line to calculate the stop line
    // normal direction. Also, if the traffic light is associated with several stop lines,
    // only the last one is actually effective.
    orientation.x = -(stop_line.point(0).y() - stop_line.point(stop_line.point_size() - 1).y());
    orientation.y = stop_line.point(0).x() - stop_line.point(stop_line.point_size() - 1).x();

    line_vertices.clear();
    const auto& point = GetMiddlePoint(stop_line.point());
    line_vertices.push_back(math::Vec3d(traffic_light_point.x(), traffic_light_point.y(),
                                        traffic_light_point.z() + GetGlLayer(kLayerTrafficLight)));
    line_vertices.push_back(
        math::Vec3d(point.x(), point.y(), point.z() + GetGlLayer(kLayerRoadGraphRoi)));
    // Draw traffic line.
    traffic_lines.SetData(
        utils::ConstArrayView<math::Vec3d>(line_vertices.data(), line_vertices.size()),
        Color::LightBlue());
    traffic_lines.Draw();
    ASSERT_GL_OK();

    math::Vec2d delta(point.x() - traffic_light_point.x(), point.y() - traffic_light_point.y());
    if (delta.InnerProd(orientation) < 0.0) {
      orientation = -orientation;
    }

    //
    // glPushMatrix
    glm::mat4 prev_modelview_mat(1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(prev_modelview_mat));

    // Calculate the new modelview matrix.
    const float radian = std::atan2(orientation.y, orientation.x);
    glm::mat4 modelview_mat = glm::translate(
        prev_modelview_mat, glm::vec3(traffic_light_point.x(), traffic_light_point.y(),
                                      traffic_light_point.z() + GetGlLayer(kLayerTrafficLight)));
    modelview_mat = glm::rotate(modelview_mat, radian, glm::vec3(0.0, 0.0, 1.0));
    // Set current model-view matrix
    glLoadMatrixf(glm::value_ptr(modelview_mat));

    // Draw traffic light model.
    traffic_light_model_.Draw();

    // glPopMatrix
    glLoadMatrixf(glm::value_ptr(prev_modelview_mat));

    /*
    // Draw traffic light id
    math::Vec3d pos(traffic_light_point.x(), traffic_light_point.y(),
                    traffic_light_point.z() + 0.5);
    font_renderer_->DrawText3D(traffic_light.id().id(), pos, Color(1.0f, 1.0f, 0.0f));
     */
  }
}

}  // namespace display
}  // namespace utils

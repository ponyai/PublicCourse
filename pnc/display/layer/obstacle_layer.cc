// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/layer/obstacle_layer.h"

#include "common/proto/agent_status.pb.h"
#include "common/proto/perception.pb.h"
#include "common/utils/containers/utils.h"
#include "common/utils/gl/basic_primitives.h"
#include "common/utils/math/box2d.h"
#include "pnc/display/gl_layers.h"

namespace utils {
namespace display {

using interface::geometry::Point3D;
using interface::perception::ObjectType;
using interface::perception::PerceptionObstacle;
using interface::agent::AgentStatus;
using utils::display::Color;

namespace {
struct LineWidth {
  static constexpr float kArrow = 2.0f;
  static constexpr float kObstacle = 2.0f;
};
}  // namespace

void ObstacleLayer::Draw() const {
  if (!user_interface_data_.perspective_vehicle.empty()) {
    for (const auto& vehicle_agent_data : data_.vehicle_agent()) {
      if (vehicle_agent_data.name() == user_interface_data_.perspective_vehicle) {
        const AgentStatus& agent_status = vehicle_agent_data.agent_status();
        for (const PerceptionObstacle& obstacle : agent_status.perception_status().obstacle()) {
          DrawObstacle(obstacle, true, true, true);
        }
      }
    }
  } else {
    for (const auto& pedestrian : data_.pedestrian()) {
      DrawObstacle(pedestrian, true, true, true);
    }
  }
}

void ObstacleLayer::DrawObstacle(const PerceptionObstacle& obstacle, bool draw_id,
                                 bool /* draw_velocity */, bool /*draw_vehicle_box*/,
                                 Color text_color, double text_height_offset) const {
  DCHECK(font_renderer_ != nullptr);
  float layer = utils::display::GetGlLayer(utils::display::kLayerObstacle);

  // Default height
  double height = 1.5;
  if (obstacle.has_height()) {
    height = obstacle.height();
  }
  auto& points = obstacle.polygon_point();
  if (points.empty()) {
    return;
  }

  // Draw obstacle id.
  std::ostringstream id_stream;
  if (draw_id) {
    id_stream << obstacle.id();
  }

  if (!id_stream.str().empty()) {
    const Point3D& first_point = points.Get(0);
    math::Vec3d pos(first_point.x(), first_point.y(), height + text_height_offset + 0.5);
    std::string id_str = id_stream.str();
    font_renderer_->DrawText3D(id_str, pos, text_color);
  }

  Color color;
  switch (obstacle.type()) {
    // Green.
    case ObjectType::CAR:
    case ObjectType::CAR_LARGE: {
      color = Color::Green();
      break;
    }
    // Yellow.
    case ObjectType::PEDESTRIAN: {
      color = Color::Yellow();
      break;
    }
    // Light blue.
    case ObjectType::CYCLIST: {
      color = Color::LightBlue();
      break;
    }
    // Purple.
    default: {
      color = Color::Purple();
      break;
    }
  }

  gl::Lines lines(context_);
  lines.GetMutableRenderingState()->line_width = LineWidth::kObstacle;

  std::vector<math::Vec3d> line_vertices;
  for (int i = 0; i < points.size(); ++i) {
    const Point3D& point = points.Get(i);
    const Point3D& next_point = points.Get((i + 1) % points.size());
    line_vertices.push_back(math::Vec3d(point.x(), point.y(), layer));
    line_vertices.push_back(math::Vec3d(point.x(), point.y(), layer + height));
    line_vertices.push_back(math::Vec3d(point.x(), point.y(), layer));
    line_vertices.push_back(math::Vec3d(next_point.x(), next_point.y(), layer));
    line_vertices.push_back(math::Vec3d(point.x(), point.y(), layer + height));
    line_vertices.push_back(math::Vec3d(next_point.x(), next_point.y(), layer + height));
  }
  lines.SetData(utils::ConstArrayView<math::Vec3d>(line_vertices.data(), line_vertices.size()),
                color);
  lines.Draw();

  if (obstacle.has_heading()) {
    const double arrow_length = 1.5;
    const math::Vec3d arrow_start(math::ToVec2d(obstacle.polygon_point(0)), layer + height);
    const math::Vec3d arrow_dir(math::Vec2d::FromLengthAndHeading(arrow_length, obstacle.heading()),
                                0.0);
    const math::Vec3d arrow_end = arrow_start + arrow_dir;

    utils::display::OpenglPainter::ArrowStyle<math::Vec3d> arrow_style;
    arrow_style.up_dir = math::Vec3d(0, 0, 1);
    arrow_style.line_style.line_width = LineWidth::kArrow;
    arrow_style.line_style.is_dash_line = false;
    arrow_style.wing_angle = 45;
    gl_painter_->DrawArrow(arrow_start, arrow_end, arrow_style);
  }
}

}  // namespace display
}  // namespace utils

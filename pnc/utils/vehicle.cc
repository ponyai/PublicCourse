// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/utils/vehicle.h"

#include "common/utils/math/transform/transform.h"
#include "pnc/utils/vehicle_status_helper.h"

namespace utils {
namespace vehicle {

using math::Box2d;
using math::Vec2d;
using interface::vehicle::VehicleParams;

Box2d GetVehicleBox(const Vec2d& center, double heading, const VehicleParams& vehicle_params) {
  const Vec2d unit = Vec2d::FromUnit(heading);
  const double ra_to_center =
      vehicle_params.vehicle_ra_to_front() - vehicle_params.vehicle_length() / 2.0;
  return Box2d(center + unit * ra_to_center, heading, vehicle_params.vehicle_length(),
               vehicle_params.vehicle_width());
}

Box2d GetVehicleBox(const interface::agent::VehicleStatus& vehicle_status,
                    const VehicleParams& vehicle_params) {
  return GetVehicleBox(Vec2d(vehicle_status.position().x(), vehicle_status.position().y()),
                       math::transform::GetYaw(vehicle_status.orientation()), vehicle_params);
}

interface::perception::PerceptionObstacle ToPerceptionObstacle(
    const interface::agent::VehicleStatus& vehicle_status,
    const interface::vehicle::VehicleParams& vehicle_params, const std::string& id) {
  Box2d box = GetVehicleBox(vehicle_status, vehicle_params);
  return ToPerceptionObstacle(box, vehicle_params.vehicle_height(), id, interface::perception::CAR);
}

interface::perception::PerceptionObstacle ToPerceptionObstacle(
    const Box2d& box, double height, const std::string& id,
    interface::perception::ObjectType type) {
  std::vector<Vec2d> points = box.GetCorners();
  interface::perception::PerceptionObstacle perception_obstacle;
  perception_obstacle.set_id(id);
  perception_obstacle.set_heading(box.HeadingVector().Angle());
  perception_obstacle.set_type(type);
  perception_obstacle.set_height(height);
  for (const Vec2d& point : points) {
    auto* added_point = perception_obstacle.add_polygon_point();
    added_point->set_x(point.x);
    added_point->set_y(point.y);
    added_point->set_z(0);
  }
  return perception_obstacle;
}

}  // namespace vehicle
}  // namespace utils

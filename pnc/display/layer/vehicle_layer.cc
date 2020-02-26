// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/layer/vehicle_layer.h"
#include "common/utils/file/path.h"
#include "common/utils/gl_support/material.h"
#include "pnc/display/gl_layers.h"

namespace utils {
namespace display {

using utils::display::Color;
using utils::gl_support::Material;

VehicleLayer::VehicleLayer(const std::string& name,
                           const interface::simulation::SimulationSystemData& data,
                           const UserInterfaceData& user_interface_data,
                           const interface::vehicle::VehicleParams& vehicle_params)
    : SimulationSystemLayerBase(name, data),
      user_interface_data_(user_interface_data),
      vehicle_params_(vehicle_params) {
  std::string model_dir = utils::path::Get3dModelsDirectory();

  vehicle_.body.LoadFromFile(file::path::Join(model_dir, "passat.obj"));
  vehicle_.velodyne.LoadFromFile(file::path::Join(model_dir, "velodyne.obj"));
  vehicle_.SetDiffuseColor(Color::Red());
}

void VehicleLayer::Draw() const {
  for (const interface::simulation::VehicleAgentData& vehicle_agent_data : data_.vehicle_agent()) {
    const interface::agent::AgentStatus& agent_status = vehicle_agent_data.agent_status();
    if (!agent_status.simulation_status().is_alive()) {
      continue;
    }
    if (!user_interface_data_.perspective_vehicle.empty()) {
      if (vehicle_agent_data.name() != user_interface_data_.perspective_vehicle) {
        continue;
      }
    }
    const interface::agent::VehicleStatus& vehicle_status = agent_status.vehicle_status();
    math::Vec3d vehicle_reference_point(vehicle_status.position().x(),
                                        vehicle_status.position().y(),
                                        GetGlLayer(utils::display::kLayerVehicle));
    DrawVehicle(vehicle_, vehicle_agent_data.name(), vehicle_reference_point,
                math::transform::ToEigen(vehicle_status.orientation()), vehicle_params_);
    if (agent_status.route_status().has_destination()) {
      DrawRouterRequest(agent_status.route_status().destination());
    }
  }
}

void VehicleLayer::DrawVehicle(const utils::display::Vehicle& vehicle,
                               const std::string& name,
                               const math::Vec3d& vehicle_reference_point,
                               const Eigen::Quaterniond& orientation,
                               const interface::vehicle::VehicleParams& vehicle_params) const {
  glPushMatrix();
  glTranslated(vehicle_reference_point.x, vehicle_reference_point.y,
               -vehicle_params.vehicle_ra_to_ground() + vehicle_reference_point.z);
  Eigen::AngleAxisd angle_axis(orientation);
  glRotated(math::RadianToDegree(angle_axis.angle()), angle_axis.axis().x(), angle_axis.axis().y(),
            angle_axis.axis().z());
  glTranslated(vehicle_params.vehicle_length() * 0.5 - vehicle_params.vehicle_ra_to_rear(), 0, 0);
  // Vehicle body.
  vehicle.Draw();

  glPopMatrix();

  // Draw traffic light id
  math::Vec3d pos(vehicle_reference_point.x, vehicle_reference_point.y,
                  vehicle_reference_point.z + 0.5);
  font_renderer_->DrawText3D(name, pos, Color(1.0f, 1.0f, 0.0f));
}

void VehicleLayer::DrawRouterRequest(const interface::geometry::Point3D& destination) const {
  constexpr float kOffset = 0.31640625f;  // center flag pole (127-46)/256
  float flag_size = 0.75f + user_interface_data_.camera_controller->distance() * 0.025f;
  flag_size = std::min(std::max(flag_size, 0.75f), 32.0f);
  float angle = user_interface_data_.camera_controller->azimuth_angle() + M_PI * 0.5;
  float offset = kOffset * flag_size * 2.0f;
  float dx = offset * std::cos(angle);
  float dy = offset * std::sin(angle);
  glm::vec2 size(flag_size, flag_size);
  glm::vec3 center(destination.x() + dx, destination.y() + dy, flag_size);
  static_cast<PncOpenglPainter*>(gl_painter_)
      ->DrawVerticalSign(center, size, angle, "flag_red.png");
}

}  // namespace display
}  // namespace utils

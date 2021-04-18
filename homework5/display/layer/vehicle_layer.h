// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/vehicle_params.pb.h"
#include "common/utils/display/color.h"
#include "homework6/display/layer/simulation_system_layer_base.h"
#include "homework6/display/pnc_gl_painter.h"
#include "homework6/display/user_interface_data.h"

namespace utils {
namespace display {

class VehicleLayer : public SimulationSystemLayerBase {
 public:
  VehicleLayer(const std::string& name, const interface::simulation::SimulationSystemData& data,
               const UserInterfaceData& user_interface_data,
               const interface::vehicle::VehicleParams& vehicle_params);
  ~VehicleLayer() override = default;

  void Draw() const override;

 private:
  void DrawVehicle(const utils::display::Vehicle& vehicle,
                   const math::Vec2d& vehicle_reference_point, const Eigen::Quaterniond& yaw,
                   const interface::vehicle::VehicleParams& vehicle_params) const;
  void DrawVehicle(const utils::display::Vehicle& vehicle,
                   const math::Vec2d& vehicle_reference_point, double heading,
                   const interface::vehicle::VehicleParams& vehicle_params) const;
  void DrawVehicle(const utils::display::Vehicle& vehicle,
                   const math::Vec3d& vehicle_reference_point, const Eigen::Quaterniond& yaw,
                   const interface::vehicle::VehicleParams& vehicle_params) const;

  void DrawRouterRequest(const interface::geometry::Point3D& destination) const;

  const interface::vehicle::VehicleParams& vehicle_params_;

  const UserInterfaceData& user_interface_data_;

  utils::display::Vehicle vehicle_;
};

}  // namespace display
}  // namespace utils

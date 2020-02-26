// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/perception.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/utils/math/box2d.h"
#include "pnc/simulation/vehicle_agent.h"

namespace utils {
namespace vehicle {

math::Box2d GetVehicleBox(const math::Vec2d& center, double heading,
                          const interface::vehicle::VehicleParams& vehicle_params);

math::Box2d GetVehicleBox(const interface::agent::VehicleStatus& vehicle_status,
                          const interface::vehicle::VehicleParams& vehicle_params);

interface::perception::PerceptionObstacle ToPerceptionObstacle(
    const interface::agent::VehicleStatus& vehicle_status,
    const interface::vehicle::VehicleParams& vehicle_params, const std::string& id);

interface::perception::PerceptionObstacle ToPerceptionObstacle(
    const math::Box2d& box, double height, const std::string& id,
    interface::perception::ObjectType type);

}  // namespace vehicle
}  // namespace utils

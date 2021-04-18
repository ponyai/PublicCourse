#pragma once

#include "common/proto/control.pb.h"
#include "common/proto/vehicle_params.pb.h"
#include "common/proto/vehicle_status.pb.h"

namespace vehicle_status_model {

class VehicleStatusModelSolver {
 public:
  explicit VehicleStatusModelSolver(const interface::vehicle::VehicleParams& vehicle_params);

  void Initialize(double initial_time,
                  const interface::agent::VehicleStatus& initial_vehicle_status);
  interface::agent::VehicleStatus UpdateVehicleStatus(double current_time,
                                                      interface::control::ControlCommand);
};

std::unique_ptr<VehicleStatusModelSolver> CreateVehicleStatusModelSolver(
    const interface::vehicle::VehicleParams& vehicle_params);
};

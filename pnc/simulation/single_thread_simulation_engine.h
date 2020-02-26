// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <chrono>
#include <vector>

#include "pnc/simulation/simulation_engine_base.h"
#include "pnc/simulation/vehicle_agent.h"
#include "pnc/simulation/vehicle_agent_factory.h"

namespace simulation {

class SingleThreadSimulationEngine : public SimulationEngineBase {
 public:
  SingleThreadSimulationEngine(const interface::simulation::SimulationConfig& simulation_config)
      : SimulationEngineBase(simulation_config) {}
  ~SingleThreadSimulationEngine() = default;

  void InitializeInternal() override;

  void RunOneIterationInternal() override;

 private:
  std::vector<std::unique_ptr<VehicleAgent>> cars_;
};
};

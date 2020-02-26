// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <unordered_map>

#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"
#include "common/proto/simulation_config.pb.h"
#include "pnc/simulation/types.h"

#include "pnc/agents/agents.h"

#include "glog/logging.h"

namespace simulation {

class SimulationEngineBase {
 public:
  explicit SimulationEngineBase(const interface::simulation::SimulationConfig& simulation_config) {
    simulation_config_.CopyFrom(simulation_config);
  }
  virtual ~SimulationEngineBase() = default;

  void Initialize() {
    CHECK(agent_status_map_set_);
    agent_status_map_set_ = false;
    InitializeInternal();
  }

  void SetAgentStatusMap(const AgentStatusMap& agent_status_map) {
    agent_status_map_ = agent_status_map;
    agent_status_map_set_ = true;
  }

  const AgentResponseMap& GetAgentResponseMap() { return agent_response_map_; }

  void RunOneIteration() {
    CHECK(agent_status_map_set_);
    agent_status_map_set_ = false;
    RunOneIterationInternal();
  }

  void Terminate() { TerminateInternal(); }

 protected:
  virtual void InitializeInternal() = 0;
  virtual void RunOneIterationInternal() = 0;
  virtual void TerminateInternal() {}

  AgentStatusMap agent_status_map_;
  AgentResponseMap agent_response_map_;
  bool agent_status_map_set_ = false;
  interface::simulation::SimulationConfig simulation_config_;
};
};

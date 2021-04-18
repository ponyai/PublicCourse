// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <unordered_map>
#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"
#include "glog/logging.h"
#include "homework6/agents/agents.h"
#include "homework6/proto/simulation_config.pb.h"

namespace simulation {

class SimulationEngineBase {
 public:
  explicit SimulationEngineBase(const interface::homework6::SimulationConfig& simulation_config) {
    simulation_config_.CopyFrom(simulation_config);
  }
  virtual ~SimulationEngineBase() = default;

  void Initialize() {
    CHECK(agent_status_map_set_);
    agent_status_map_set_ = false;
    InitializeInternal();
  }

  void SetAgentStatusMap(
      const std::unordered_map<std::string, interface::agent::AgentStatus>& agent_status_map) {
    agent_status_map_ = agent_status_map;
    agent_status_map_set_ = true;
  }

  const std::unordered_map<std::string, interface::control::ControlCommand>&
  GetControlCommandMap() {
    return control_command_map_;
  }

  void RunOneIteration() {
    CHECK(agent_status_map_set_);
    agent_status_map_set_ = false;
    RunOneIterationInternal();
  }

 protected:
  virtual void InitializeInternal() = 0;
  virtual void RunOneIterationInternal() = 0;

  std::unordered_map<std::string, interface::agent::AgentStatus> agent_status_map_;
  std::unordered_map<std::string, interface::control::ControlCommand> control_command_map_;

  bool agent_status_map_set_ = false;

  interface::homework6::SimulationConfig simulation_config_;
};
};

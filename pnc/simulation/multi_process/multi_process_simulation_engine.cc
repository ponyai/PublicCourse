// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/multi_process/multi_process_simulation_engine.h"

namespace simulation {

void MultiProcessSimulationEngine::InitializeInternal() {
  server_thread_ = std::thread([this]() {
    server_ = std::make_unique<TcpSimulationServer>(kLocalHost, kPort, num_agents_);
    CHECK_OK(server_->Initialize());
    CHECK_OK(server_->Start());
  });
  for (const interface::simulation::AgentConfig& agent_config : simulation_config_.agent_config()) {
    multi_process_->StartWorker([&agent_config, this]() { StartClient(simulation_config_); });
  }
  server_thread_.join();
  server_->SetAgentStatusMap(agent_status_map_);
  CHECK_OK(server_->InitializeAgents());
}

void MultiProcessSimulationEngine::RunOneIterationInternal() {
  server_->SetAgentStatusMap(agent_status_map_);
  CHECK_OK(server_->RunOneIteration());
  agent_response_map_ = server_->GetAgentResponseMap();
}

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_response.pb.h"
#include "pnc/parallelism/multi_process.h"
#include "pnc/simulation/multi_process/server_client_defines.h"
#include "pnc/simulation/multi_process/tcp_agent_client.h"
#include "pnc/simulation/multi_process/tcp_simulation_server.h"
#include "pnc/simulation/simulation_engine_base.h"

namespace simulation {

class MultiProcessSimulationEngine : public SimulationEngineBase {
 public:
  explicit MultiProcessSimulationEngine(
      const interface::simulation::SimulationConfig& simulation_config)
      : SimulationEngineBase(simulation_config) {
    num_agents_ = simulation_config.agent_config_size();
    multi_process_ = std::make_unique<parallelism::MultiProcessParallelizer>(num_agents_);
  }
  ~MultiProcessSimulationEngine() override {
    multi_process_->KillAllWorkers();
    CHECK_OK(server_->Terminate());
  }

  void InitializeInternal() override;
  void RunOneIterationInternal() override;

 private:
  void StartClient(const interface::simulation::SimulationConfig& simulation_config) {
    std::unique_ptr<TcpAgentClient> tcp_agent_client =
        std::make_unique<TcpAgentClient>(kLocalHost, kPort, simulation_config);
    CHECK_OK(tcp_agent_client->Initialize());
    CHECK_OK(tcp_agent_client->Start());
    CHECK_OK(tcp_agent_client->Terminate());
  }
  std::unique_ptr<TcpSimulationServer> server_;
  std::thread server_thread_;
  std::unique_ptr<parallelism::MultiProcessParallelizer> multi_process_;
  int num_agents_ = 0;
};
};

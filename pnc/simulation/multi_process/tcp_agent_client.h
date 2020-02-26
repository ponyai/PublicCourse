// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <google/protobuf/message.h>
#include <netinet/tcp.h>

#include "pnc/simulation/multi_process/net_client_interface.h"
#include "pnc/simulation/multi_process/tcp_connection.h"
#include "pnc/simulation/vehicle_agent.h"

#include "common/proto/agent_comms.pb.h"
#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"
#include "common/proto/simulation_config.pb.h"

namespace simulation {
class TcpAgentClient : public NetClientInterface {
 public:
  TcpAgentClient(const std::string& ip_address, int port,
                 const interface::simulation::SimulationConfig& simulation_config);
  ~TcpAgentClient() override = default;

  utils::Status Initialize() override;
  utils::Status Start() override;
  utils::Status Terminate() override;

  utils::Status ClearRecvBuffer() override;
  utils::Status BlockingSend(const uint8_t* data, int size) override;
  utils::Status BlockingReceive(uint8_t* data, int size) override;
  utils::Status BlockingSendMessage(const google::protobuf::Message& message);
  utils::Status BlockingReceiveMessage(google::protobuf::Message* message);

 private:
  std::string ip_address_;
  int port_ = -1;

  std::unique_ptr<TcpConnection> connection_;
  std::unique_ptr<VehicleAgent> vehicle_agent_;

  interface::agent::AgentStatus agent_status_;
  interface::simulation::SimulationConfig simulation_config_;
};

}  // namespace simulation

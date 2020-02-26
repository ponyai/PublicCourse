// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <atomic>
#include <thread>
#include <unordered_map>

#include "common/proto/agent_response.pb.h"
#include "common/utils/common/mutex.h"
#include "pnc/parallelism/thread_pool.h"
#include "pnc/simulation/multi_process/message_receiver.h"
#include "pnc/simulation/multi_process/net_server_interface.h"
#include "pnc/simulation/multi_process/tcp_agent_client.h"
#include "pnc/simulation/multi_process/tcp_connection.h"
#include "pnc/simulation/types.h"

namespace simulation {

class TcpSimulationServer : public NetServerInterface {
 public:
  enum class ServerStatus {
    kUninitialized,
    kInitialized,
    kStarted,
    kTerminated,
  };

  TcpSimulationServer(const std::string& ip_address, int port, int num_agents);
  ~TcpSimulationServer() override;

  utils::Status AcceptNewConnection(int* socket_fd);
  utils::Status Initialize() override;
  utils::Status Start() override;
  utils::Status Terminate() override;

  utils::Status InitializeAgents();
  utils::Status RunOneIteration();

  ServerStatus status() const { return status_; }

  void SetAgentStatusMap(const AgentStatusMap& agent_status_map) {
    agent_status_map_ = agent_status_map;
  }

  const AgentResponseMap& GetAgentResponseMap() {
    utils::MutexLocker lock(&mutex_);
    return agent_response_map_;
  }

  utils::Status SendRequest(TcpConnection* tcp_connection,
                            const interface::comms::Request& request);

 private:
  utils::Mutex mutex_{"TcpSimulationServer"};

  std::string ip_address_;
  int port_ = -1;
  int num_agents_ = -1;
  ServerStatus status_ = ServerStatus::kUninitialized;
  int listen_fd_ = -1;
  parallelism::ThreadPool pool_;
  std::atomic<bool> keep_running_{false};

  AgentStatusMap agent_status_map_;
  AgentResponseMap agent_response_map_ GUARDED_BY(mutex_);
  std::unordered_map<std::string, TcpConnection*> tcp_connection_map_;
  std::vector<std::unique_ptr<TcpConnection>> tcp_connections_;
  std::unordered_map<std::string, std::unique_ptr<MessageReceiver>> message_receiver_map_;
  std::unordered_map<std::string, bool> initialization_map_;
};

}  // namespace simulation

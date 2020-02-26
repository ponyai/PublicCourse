// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/multi_process/tcp_simulation_server.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common/utils/common/mutex.h"
#include "pnc/simulation/manual_test_agent.h"

namespace simulation {

TcpSimulationServer::TcpSimulationServer(const std::string& ip_address, int port, int num_agents)
    : ip_address_(ip_address), port_(port), num_agents_(num_agents), pool_(num_agents) {}

TcpSimulationServer::~TcpSimulationServer() {
  if (status_ != ServerStatus::kTerminated) {
    CHECK_OK(Terminate());
  }
}

utils::Status TcpSimulationServer::Initialize() {
  CHECK(status_ == ServerStatus::kUninitialized);
  CHECK_EQ(listen_fd_, -1);
  status_ = ServerStatus::kInitialized;
  listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd_ == -1) {
    return utils::Status::Error("Failed to open the socket.");
  }
  int enable = 1;
  if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    return utils::Status::Error("setsockopt(SO_REUSEADDR) failed.");
  }

  sockaddr_in serv_addr{};
  int ret = 0;
  serv_addr.sin_family = AF_INET;
  if (!ip_address_.empty()) {
    ret = ::inet_pton(AF_INET, ip_address_.c_str(), &(serv_addr.sin_addr));
    if (ret != 1) {
      return utils::Status::Error("Invalid IP address: " + ip_address_ + ", error code: " +
                                  std::to_string(errno));
    }
  } else {
    serv_addr.sin_addr.s_addr = INADDR_ANY;
  }
  serv_addr.sin_port = htons(port_);

  ret = ::bind(listen_fd_, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr));
  if (ret != 0) {
    return utils::Status::Error("Failed to bind to " + ip_address_ + ":" + std::to_string(port_) +
                                ", error code: " + std::to_string(errno));
  }

  ret = ::listen(listen_fd_, num_agents_);
  if (ret != 0) {
    return utils::Status::Error("Failed to listen, error code: " + std::to_string(errno));
  }

  return utils::Status::OK;
}

utils::Status TcpSimulationServer::AcceptNewConnection(int* socket_fd) {
  sockaddr_in client_addr{};
  socklen_t client_addr_len = sizeof(client_addr);
  *socket_fd = ::accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
  if (*socket_fd == -1) {
    // We might have already terminated the server.
    if (status_ != ServerStatus::kTerminated) {
      return utils::Status::Error("Failed to accept connection, error code: " +
                                  std::to_string(errno));
    }
  }
  int enable = 1;
  if (setsockopt(*socket_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0) {
    if (status_ != ServerStatus::kTerminated) {
      return utils::Status::Error("setsockopt(TCP_NODELAY) failed.");
    }
  }
  return utils::Status::OK;
}

utils::Status TcpSimulationServer::Start() {
  CHECK(status_ == ServerStatus::kInitialized);
  status_ = ServerStatus::kStarted;

  for (int i = 0; i < num_agents_; i++) {
    int socket_fd;
    LOG(INFO) << "Try to accept new connection...";
    RETURN_IF_ERROR(AcceptNewConnection(&socket_fd));
    LOG(INFO) << "New connection is accepted on fd: " << socket_fd;
    tcp_connections_.push_back(std::make_unique<TcpConnection>(socket_fd));
  }

  return utils::Status::OK;
}

utils::Status TcpSimulationServer::Terminate() {
  CHECK(status_ != ServerStatus::kTerminated);
  int ret = ::shutdown(listen_fd_, 2);
  if (ret != 0) {
    if (errno != ENOTCONN) {
      LOG(ERROR) << "Failed to shutdown the socket, error code: " << std::to_string(errno);
    }
    // Otherwise, we failed to create the connection.
  }
  ret = ::close(listen_fd_);
  if (ret != 0) {
    LOG(ERROR) << "Failed to close the socket, error code: " << std::to_string(errno);
  }
  return utils::Status::OK;
}

utils::Status TcpSimulationServer::SendRequest(TcpConnection* tcp_connection,
                                               const interface::comms::Request& request) {
  return tcp_connection->BlockingSendMessage(request);
}

utils::Status TcpSimulationServer::InitializeAgents() {
  int index_tcp_connections = 0;
  for (const auto& iter : agent_status_map_) {
    const std::string& name = iter.first;
    const interface::agent::AgentStatus& agent_status = iter.second;
    TcpConnection* tcp_connection = tcp_connections_[index_tcp_connections++].get();
    tcp_connection_map_[name] = tcp_connection;
    interface::comms::Request request;
    request.mutable_initialize_request()->mutable_agent_status()->CopyFrom(agent_status);
    request.mutable_initialize_request()->set_name(name);
    utils::Status request_status = SendRequest(tcp_connection, request);
    if (request_status.ok()) {
      // Taking data transmission time into consideration, the server waits a client's response for
      // at most one second
      message_receiver_map_[name] = std::make_unique<MessageReceiver>(tcp_connection, 1.0);
      message_receiver_map_[name]->StartReceive();
      utils::Optional<interface::comms::Response> response =
          message_receiver_map_[name]->ReceiveWithTimeout();
      if (response != utils::none) {
        initialization_map_[name] = true;
      } else {
        LOG(ERROR) << "Server does not receive agent: " << name
                   << "'s initialization response in one seconds";
      }
    } else {
      LOG(ERROR) << "request error:" << request_status.error_message();
    }
  }
  CHECK(index_tcp_connections == num_agents_);
  return utils::Status::OK;
}

utils::Status TcpSimulationServer::RunOneIteration() {
  {
    utils::MutexLocker lock(&mutex_);
    agent_response_map_.clear();
  }
  for (const auto& iter : agent_status_map_) {
    const std::string& name = iter.first;
    if (name == kKeyboardControlledAgentName) {
      continue;
    }
    const interface::agent::AgentStatus& agent_status = iter.second;
    if (agent_status.simulation_status().is_alive()) {
      TcpConnection* tcp_connection = tcp_connection_map_[name];
      MessageReceiver* message_receiver = message_receiver_map_[name].get();
      pool_.Schedule([&name, &agent_status, &tcp_connection, &message_receiver, this]() {
        interface::comms::Request request;
        request.mutable_iteration_request()->mutable_agent_status()->CopyFrom(agent_status);
        request.mutable_iteration_request()->set_name(name);
        utils::Status request_status = SendRequest(tcp_connection, request);
        if (request_status.ok()) {
          utils::Optional<interface::comms::Response> response =
              message_receiver->ReceiveWithTimeout();
          if (response != utils::none) {
            {
              utils::MutexLocker lock(&mutex_);
              agent_response_map_[name].mutable_control_command()->CopyFrom(
                  response.get().iteration_response().control_command());
              agent_response_map_[name].set_iteration_time_cost(
                  response.get().iteration_response().time_cost());
              agent_response_map_[name].mutable_debug_variables()->CopyFrom(
                  response.get().iteration_response().debug_variables());
            }
          } else {
            LOG(ERROR) << "Server does not receive agent: " << name
                       << "'s iteration response in one seconds";
          }
        } else {
          LOG(ERROR) << "request error:" << request_status.error_message();
        }
      });
    }
    pool_.WaitForWorkItemsDone();
  }
  return utils::Status::OK;
}

}  // namespace simulation

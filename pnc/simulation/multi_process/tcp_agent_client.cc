// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/multi_process/tcp_agent_client.h"

#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

#include "pnc/simulation/vehicle_agent_factory.h"

namespace simulation {

TcpAgentClient::TcpAgentClient(const std::string& ip_address, int port,
                               const interface::simulation::SimulationConfig& simulation_config)
    : ip_address_(ip_address), port_(port) {
  CHECK(!ip_address.empty());
  CHECK_GE(port, 0);
  simulation_config_.CopyFrom(simulation_config);
}

utils::Status TcpAgentClient::Initialize() {
  int socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    return utils::Status::Error("Failed to open the socket.");
  }
  int enable = 1;
  if (setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0) {
    return utils::Status::Error("setsockopt(TCP_NODELAY) failed.");
  }

  auto connection = std::make_unique<TcpConnection>(socket_fd);

  sockaddr_in destination{};
  destination.sin_addr.s_addr = ::inet_addr(ip_address_.c_str());
  if (destination.sin_addr.s_addr == INADDR_NONE) {
    return utils::Status::Error("Invalid IP address: " + ip_address_);
  }
  destination.sin_port = htons(port_);
  destination.sin_family = AF_INET;

  int connect_ret =
      ::connect(socket_fd, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
  int retry_count = 0;
  while (connect_ret != 0) {
    retry_count++;
    if (retry_count > 3) {
      break;
    }
    LOG(ERROR) << "Fail to connnect, sleeping for 1000ms";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    connect_ret =
        ::connect(socket_fd, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
  }
  if (connect_ret != 0) {
    return utils::Status::Error("Failed to connect to " + ip_address_ + ":" +
                                std::to_string(port_) + ", error code: " + std::to_string(errno));
  }

  connection_ = std::move(connection);
  return utils::Status::OK;
}

utils::Status TcpAgentClient::Start() {
  while (1) {
    interface::comms::Request request;
    RETURN_IF_ERROR(connection_->BlockingReceiveMessage(&request));
    if (request.has_initialize_request()) {
      agent_status_.CopyFrom(request.initialize_request().agent_status());
      std::string agent_name = request.initialize_request().name();
      std::string class_name;
      for (const auto& agent_config : simulation_config_.agent_config()) {
        if (agent_config.name() == agent_name) {
          class_name = agent_config.type();
          break;
        }
      }
      CHECK(!class_name.empty());
      vehicle_agent_ = std::unique_ptr<VehicleAgent>(
          VehicleAgentFactory::Instance()->Create(class_name, agent_name));
      auto start = std::chrono::system_clock::now();
      vehicle_agent_->Initialize(agent_status_);
      auto end = std::chrono::system_clock::now();
      interface::comms::Response response;
      std::chrono::duration<double> diff = end - start;
      response.mutable_initialize_response()->set_time_cost(diff.count());
      RETURN_IF_ERROR(connection_->BlockingSendMessage(response));
    }
    if (request.has_iteration_request()) {
      agent_status_.CopyFrom(request.iteration_request().agent_status());
      if (!agent_status_.simulation_status().is_alive()) {
        break;
      }
      CHECK(request.iteration_request().name() == vehicle_agent_->name());
      vehicle_agent_->clear_debug_variables();
      interface::comms::Response response;
      auto start = std::chrono::system_clock::now();
      interface::control::ControlCommand control_command =
          vehicle_agent_->RunOneIteration(agent_status_);
      auto end = std::chrono::system_clock::now();
      std::chrono::duration<double> diff = end - start;
      response.mutable_iteration_response()->set_time_cost(diff.count());
      response.mutable_iteration_response()->mutable_control_command()->CopyFrom(control_command);
      response.mutable_iteration_response()->mutable_debug_variables()->CopyFrom(
          vehicle_agent_->debug_variables());
      RETURN_IF_ERROR(connection_->BlockingSendMessage(response));
    }
  }
  return utils::Status::OK;
}

utils::Status TcpAgentClient::Terminate() {
  connection_.reset();
  return utils::Status::OK;
}

utils::Status TcpAgentClient::ClearRecvBuffer() {
  CHECK(connection_ != nullptr);
  return connection_->ClearRecvBuffer();
}

utils::Status TcpAgentClient::BlockingSend(const uint8_t* data, int size) {
  CHECK(connection_ != nullptr);
  return connection_->BlockingSend(data, size);
}

utils::Status TcpAgentClient::BlockingReceive(uint8_t* data, int size) {
  CHECK(connection_ != nullptr);
  return connection_->BlockingReceive(data, size);
}

utils::Status TcpAgentClient::BlockingSendMessage(const google::protobuf::Message& message) {
  CHECK(connection_ != nullptr);
  return connection_->BlockingSendMessage(message);
}

utils::Status TcpAgentClient::BlockingReceiveMessage(google::protobuf::Message* message) {
  CHECK(connection_ != nullptr);
  return connection_->BlockingReceiveMessage(message);
}

}  // namespace simulation

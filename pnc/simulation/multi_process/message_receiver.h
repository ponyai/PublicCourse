// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_comms.pb.h"
#include "common/utils/common/mutex.h"
#include "common/utils/common/optional.h"
#include "pnc/simulation/multi_process/tcp_connection.h"

#include <thread>

namespace simulation {

class MessageReceiver {
 public:
  MessageReceiver(TcpConnection* tcp_connection, double timeout) {
    tcp_connection_ = tcp_connection;
    timeout_ = duration(int64_t(timeout * 10000000));
  }
  ~MessageReceiver() {
    keep_running_ = false;
    receive_thread_.join();
  }

  void StartReceive();

  utils::Optional<interface::comms::Response> ReceiveWithTimeout();

 private:
  utils::Mutex mutex_{"MessageReceiver"};

  using rep = int64_t;
  using period = std::ratio<1, 10000000>;
  using duration = std::chrono::duration<rep, period>;

  duration timeout_;
  TcpConnection* tcp_connection_ = nullptr;
  utils::Optional<interface::comms::Response> response_ GUARDED_BY(mutex_);
  std::thread receive_thread_;

  std::atomic<bool> keep_running_{true};
};
};

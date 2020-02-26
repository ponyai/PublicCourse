// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/multi_process/message_receiver.h"

#include <thread>

namespace simulation {

void MessageReceiver::StartReceive() {
  receive_thread_ = std::thread([this]() {
    while (keep_running_) {
      interface::comms::Response response;
      utils::Status status = tcp_connection_->BlockingReceiveMessage(&response);
      if (status.ok()) {
        {
          utils::MutexLocker lock(&mutex_);
          response_ = response;
        }
      }
    }
  });
}

utils::Optional<interface::comms::Response> MessageReceiver::ReceiveWithTimeout() {
  utils::MutexLocker lock(&mutex_);
  lock.AwaitWithTimeout([this]() REQUIRES(mutex_) { return response_ != utils::none; }, timeout_);
  if (response_ == utils::none) {
    keep_running_ = false;
  }
  auto result = response_;
  response_ = utils::none;
  return result;
}

}  // namespace simulation

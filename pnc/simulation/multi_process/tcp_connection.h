// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <google/protobuf/message.h>
#include <netinet/tcp.h>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

#include "common/utils/status/status.h"

namespace simulation {

class TcpConnection {
 public:
  explicit TcpConnection(int socket_fd);
  ~TcpConnection();

  utils::Status ClearRecvBuffer();
  utils::Status BlockingSend(const uint8_t* data, int size);
  utils::Status BlockingReceive(uint8_t* data, int size);
  utils::Status BlockingSendMessage(const google::protobuf::Message& message);
  utils::Status BlockingReceiveMessage(google::protobuf::Message* message);

 private:
  int socket_fd_ = -1;
  int kInitialBufferSize = 4096;
  std::vector<uint8_t> buffer_;
};

}  // namespace simulation

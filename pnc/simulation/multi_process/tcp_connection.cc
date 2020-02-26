// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/multi_process/tcp_connection.h"

namespace simulation {

TcpConnection::TcpConnection(int socket_fd) : socket_fd_(socket_fd), buffer_(kInitialBufferSize) {}

TcpConnection::~TcpConnection() {
  // We might have not established the connection before deconstructor is called. So, shutdown
  // might return an error code. But, we don't really care here.
  ::shutdown(socket_fd_, 2);
  int ret = ::close(socket_fd_);
  if (ret != 0) {
    LOG(ERROR) << "Failed to close the socket, error code: " << std::to_string(errno);
  }
}

utils::Status TcpConnection::ClearRecvBuffer() {
  constexpr int kBufferSize = 4096;
  uint8_t data[kBufferSize];
  while (true) {
    int num_bytes = ::recv(socket_fd_, data, kBufferSize, MSG_DONTWAIT);
    if (num_bytes < 0) {
      if (errno == EAGAIN) {
        return utils::Status::OK;
      }
      return utils::Status::Error("Failed to clear recv buffer of the socket. Error code: " +
                                  std::to_string(errno));
    } else if (num_bytes == 0) {
      // Connection is closed. But, we don't care any more.
      return utils::Status::OK;
    }
  }
}

utils::Status TcpConnection::BlockingReceive(uint8_t* data, int size) {
  if (size == 0) {
    return utils::Status::OK;
  }
  CHECK_GT(size, 0);

  for (int i = 0; i < size;) {
    int num_bytes = ::recv(socket_fd_, data + i, size - i, 0);
    if (num_bytes == -1) {
      return utils::Status::Error("Failed to receive data through socket, error code: " +
                                  std::to_string(errno));
    } else if (num_bytes == 0) {
      // Connection is closed;
      return utils::Status::Error("Connection is closed.");
    }
    i += num_bytes;
  }
  return utils::Status::OK;
}

utils::Status TcpConnection::BlockingSend(const uint8_t* data, int size) {
  if (size == 0) {
    return utils::Status::OK;
  }
  CHECK_GT(size, 0);

  for (int i = 0; i < size;) {
    int num_bytes = ::send(socket_fd_, data + i, size - i, MSG_NOSIGNAL);
    if (num_bytes == -1) {
      if (errno == EPIPE) {
        return utils::Status::Error("Connection is closed.");
      }
      return utils::Status::Error("Failed to send data through socket, error code: " +
                                  std::to_string(errno));
    }
    i += num_bytes;
  }
  return utils::Status::OK;
}

utils::Status TcpConnection::BlockingSendMessage(const google::protobuf::Message& message) {
  int size = message.ByteSize() + 4;
  buffer_.reserve(size);
  google::protobuf::io::ArrayOutputStream raw_stream(buffer_.data(), size);
  google::protobuf::io::CodedOutputStream coded_output(&raw_stream);
  coded_output.WriteVarint32(message.ByteSize());
  CHECK(message.SerializeToCodedStream(&coded_output));
  return BlockingSend(buffer_.data(), size);
}

utils::Status TcpConnection::BlockingReceiveMessage(google::protobuf::Message* message) {
  uint8_t size_buf[4];
  google::protobuf::uint32 size;
  RETURN_IF_ERROR(BlockingReceive(size_buf, 4));
  google::protobuf::io::ArrayInputStream size_input(size_buf, 4);
  google::protobuf::io::CodedInputStream coded_size_input(&size_input);
  coded_size_input.ReadVarint32(&size);
  int offset = coded_size_input.CurrentPosition();
  buffer_.reserve(size + 4);
  std::copy_n(size_buf, 4, buffer_.data());
  RETURN_IF_ERROR(BlockingReceive(buffer_.data() + 4, size));
  google::protobuf::io::ArrayInputStream message_input(buffer_.data() + offset, size);
  google::protobuf::io::CodedInputStream coded_message_input(&message_input);
  auto limit = coded_message_input.PushLimit(size);
  CHECK(message->ParseFromCodedStream(&coded_message_input));
  coded_message_input.PopLimit(limit);
  return utils::Status::OK;
}

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cstdint>
#include <string>

#include "common/utils/containers/array_view.h"
#include "common/utils/status/status.h"

namespace simulation {

class NetClientInterface {
 public:
  NetClientInterface() = default;
  virtual ~NetClientInterface() = default;

  virtual utils::Status Initialize() = 0;
  virtual utils::Status Start() = 0;
  virtual utils::Status Terminate() = 0;

  virtual utils::Status ClearRecvBuffer() = 0;
  virtual utils::Status BlockingSend(const uint8_t* data, int size) = 0;
  virtual utils::Status BlockingReceive(uint8_t* data, int size) = 0;
};

}  // namespace simulation

// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/containers/array_view.h"
#include "common/utils/status/status.h"

namespace simulation {

class NetServerInterface {
 public:
  NetServerInterface() = default;
  virtual ~NetServerInterface() = default;

  virtual utils::Status Initialize() = 0;
  virtual utils::Status Start() = 0;
  virtual utils::Status Terminate() = 0;
};

}  // namespace simulation

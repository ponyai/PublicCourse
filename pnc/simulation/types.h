// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_response.pb.h"
#include "common/proto/agent_status.pb.h"
#include "common/proto/control.pb.h"

#include <string>

namespace simulation {

using AgentStatusMap = std::unordered_map<std::string, interface::agent::AgentStatus>;
using AgentResponseMap = std::unordered_map<std::string, interface::agent::AgentResponse>;
}

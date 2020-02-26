// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_status.pb.h"
#include "common/proto/map.pb.h"
#include "common/proto/map_id.pb.h"

namespace simulation {

class TrafficLightAgent {
 public:
  explicit TrafficLightAgent(const interface::map::Map& map) {
    map_.CopyFrom(map);
    for (const interface::map::TrafficLight& traffic_light : map.traffic_light()) {
      const interface::map::Id& id = traffic_light.id();
      if (id.id().find("E") != std::string::npos || id.id().find("W") != std::string::npos) {
        traffic_light_group_[0].push_back(id);
      } else {
        traffic_light_group_[1].push_back(id);
      }
    }
    last_state_change_time_ = -1000.0;
    // SetCurrentTime(0.0);
  }

  void SetCurrentTime(double time);

  const interface::perception::PerceptionTrafficLightStatus& GetTrafficLightStatus() {
    return traffic_light_status_;
  }

  bool StatusRefreshed() { return status_refreshed; }

 private:
  interface::map::Map map_;
  double current_time_;

  interface::perception::PerceptionTrafficLightStatus traffic_light_status_;
  int current_index_ = 0;
  double last_state_change_time_ = 0.0;
  std::vector<interface::map::Id> traffic_light_group_[2];
  bool status_refreshed = false;
};
};

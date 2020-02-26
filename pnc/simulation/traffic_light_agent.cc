// Copyright @2018 Pony AI Inc. All rights reserved.

#include <iomanip>

#include "glog/logging.h"
#include "common/utils/math/common.h"
#include "pnc/simulation/traffic_light_agent.h"

namespace {

constexpr int kNumIntervals = 4;
constexpr int kTimeInterval[4] = {20, 3, 20, 3};
constexpr interface::map::Bulb::Color kColorSlots[4] = {
    interface::map::Bulb::RED, interface::map::Bulb::YELLOW, interface::map::Bulb::GREEN,
    interface::map::Bulb::YELLOW};

};  // namespace anonymous

namespace simulation {

void TrafficLightAgent::SetCurrentTime(double time) {
  current_time_ = time;
  if (current_time_ - last_state_change_time_ > kTimeInterval[current_index_] - math::kEpsilon) {
    status_refreshed = true;
    last_state_change_time_ = current_time_;
    current_index_++;
    current_index_ %= kNumIntervals;
    traffic_light_status_.clear_single_traffic_light_status();
    for (const interface::map::Id& id : traffic_light_group_[0]) {
      interface::perception::SingleTrafficLightStatus* single_traffic_light_status =
          traffic_light_status_.add_single_traffic_light_status();
      single_traffic_light_status->mutable_id()->CopyFrom(id);
      single_traffic_light_status->set_color(kColorSlots[current_index_]);
    }
    for (const interface::map::Id& id : traffic_light_group_[1]) {
      interface::perception::SingleTrafficLightStatus* single_traffic_light_status =
          traffic_light_status_.add_single_traffic_light_status();
      single_traffic_light_status->mutable_id()->CopyFrom(id);
      single_traffic_light_status->set_color(kColorSlots[(current_index_ + 2) % kNumIntervals]);
    }
  } else {
    status_refreshed = false;
  }
}

}  // namespace simulation

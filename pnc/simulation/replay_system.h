// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/simulation.pb.h"
#include "common/utils/common/mutex.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "pnc/simulation/system_base.h"
#include "pnc/utils/index.h"

namespace simulation {

class ReplaySystem : public SystemBase {
 public:
  struct SimulationStatus {
    bool playback_paused = false;
    double playback_speed_x = 1.0;
    bool playback_terminated = false;
  };

  explicit ReplaySystem(const std::string& record_filepath): record_filepath_(record_filepath) {}

  void Initialize() {}

  void Start();

  interface::simulation::SimulationSystemData FetchData() override;

 private:
  std::string record_filepath_;

  utils::Mutex mutex_{"replay"};
  double played_time_ = 0.0;
  interface::simulation::SimulationSystemData simulation_frame_data_ GUARDED_BY(mutex_);
};

};  // namespace simulation

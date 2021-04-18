// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/common/mutex.h"

#include <queue>

#include "common/proto/playback.pb.h"
#include "common/proto/simulation.pb.h"

namespace simulation {

class SystemBase {
 public:
  SystemBase() = default;
  virtual ~SystemBase() = default;

  struct PlaybackStatus {
    bool playback_paused = false;
    double playback_speed_x = 1.0;
    bool playback_terminated = false;
    int next_iteration_count = 0;
  };

  void PushPauseCommand();
  void PushSimulationSpeedCommand(double x);
  void PushPlaybackTerminateCommand();
  void FlushPlaybackCommands();
  void PushPlaybackNextOneIterationCommand();

  PlaybackStatus playback_status() { return playback_status_; }

 protected:
  virtual interface::simulation::SimulationSystemData FetchData() = 0;
  bool MaybePlaybackNextOneIteration();

 private:
  utils::Mutex mutex_{"playback_mutex"};
  PlaybackStatus playback_status_;

  std::queue<interface::playback::PlaybackCommand> playback_command_queue_ GUARDED_BY(mutex_);
};

};  // namespace simulation

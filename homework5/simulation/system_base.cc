// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/simulation/system_base.h"
#include "glog/logging.h"

namespace simulation {

void SystemBase::PushPauseCommand() {
  interface::playback::PlaybackCommand cmd;
  cmd.mutable_playback_pause();
  utils::MutexLocker lock(&mutex_);
  playback_command_queue_.push(cmd);
}

void SystemBase::PushSimulationSpeedCommand(double x) {
  interface::playback::PlaybackCommand cmd;
  cmd.set_playback_speed_x(x);
  utils::MutexLocker lock(&mutex_);
  playback_command_queue_.push(cmd);
}

void SystemBase::PushPlaybackTerminateCommand() {
  interface::playback::PlaybackCommand cmd;
  cmd.mutable_playback_terminate();
  utils::MutexLocker lock(&mutex_);
  playback_command_queue_.push(cmd);
}

void SystemBase::PushPlaybackNextOneIterationCommand() {
  interface::playback::PlaybackCommand cmd;
  cmd.mutable_playback_next_iteration();
  utils::MutexLocker lock(&mutex_);
  playback_command_queue_.push(cmd);
}

void SystemBase::FlushPlaybackCommands() {
  utils::MutexLocker lock(&mutex_);
  while (!playback_command_queue_.empty()) {
    interface::playback::PlaybackCommand command = playback_command_queue_.front();
    playback_command_queue_.pop();
    if (command.has_playback_pause()) {
      if (!playback_status_.playback_paused) {
        playback_status_.next_iteration_count = 0;
      }
      playback_status_.playback_paused ^= 1;
    }
    if (command.has_playback_speed_x()) {
      playback_status_.playback_speed_x *= command.playback_speed_x();
      playback_status_.playback_speed_x = std::max(0.1, playback_status_.playback_speed_x);
      playback_status_.playback_speed_x = std::min(10.0, playback_status_.playback_speed_x);
    }
    if (command.has_playback_terminate()) {
      playback_status_.playback_terminated = true;
    }
    if (command.has_playback_next_iteration()) {
      playback_status_.next_iteration_count++;
    }
  }
}

bool SystemBase::MaybePlaybackNextOneIteration() {
  if (playback_status_.next_iteration_count > 0) {
    playback_status_.next_iteration_count--;
    return true;
  }
  return false;
}

}  // namespace simulation

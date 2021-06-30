// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/simulation/replay_system.h"
#include <thread>

namespace {

bool ReadProto(file::FileInterface* file, google::protobuf::Message* proto) {
  uint32_t number;
  uint64_t size;
  if (file->Read(&number, sizeof(number)) != sizeof(number) ||
      file->Read(&size, sizeof(size)) != sizeof(size)) { return false; }
  std::string buffer;
  buffer.resize(size);
  if (file->Read(&buffer[0], size) != size) { return false; }
  proto->ParseFromString(buffer);
  return true;
}

}  // namespace

namespace simulation {

void ReplaySystem::Start() {
  LOG(INFO) << "Replay system is started";

  std::string perception_filename = file::path::Join(record_filepath_, "perception.proto.bin");
  std::string localization_filename = file::path::Join(record_filepath_, "localization.proto.bin");
  std::string traffic_light_filename = file::path::Join(record_filepath_, "trafficlight.proto.bin");
  std::unique_ptr<file::FileInterface> perception_log_file =
      std::make_unique<file::File>(perception_filename, "r");
  std::unique_ptr<file::FileInterface> localization_log_file =
      std::make_unique<file::File>(localization_filename, "r");
  std::unique_ptr<file::FileInterface> traffic_light_log_file =
      std::make_unique<file::File>(traffic_light_filename, "r");

  interface::agent::LocalizationResult localization;
  interface::perception::PerceptionObstacles obstacles;
  interface::traffic_light::TrafficLightDetection traffic_light;

  if (!ReadProto(perception_log_file.get(), &obstacles)) { return; }
  if (!ReadProto(localization_log_file.get(), &localization)) { return; }
  if (!ReadProto(traffic_light_log_file.get(), &traffic_light)) { return; }

  double start_time = localization.sensor_timestamp();

  while (true) {
    auto system_time = std::chrono::steady_clock::now();
    FlushPlaybackCommands();
    if (playback_status().playback_seek_to_timestamp > 0) {
      playback_status_.playback_seek_to_timestamp = -1.0;
    }
    if (playback_status().playback_paused) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    while (start_time + played_time_ > localization.sensor_timestamp())
    if (!ReadProto(localization_log_file.get(), &localization)) { return; }

    while (start_time + played_time_ > obstacles.master_lidar_measurement_timestamp())
    if (!ReadProto(perception_log_file.get(), &obstacles)) { return; }

    while (start_time + played_time_ > traffic_light.camera_group_timestamp())
    if (!ReadProto(traffic_light_log_file.get(), &traffic_light)) { return; }

    {
      utils::MutexLocker lock(&mutex_);
      *simulation_frame_data_.mutable_vehicle_localization() = localization;
      *simulation_frame_data_.mutable_obstacles() = obstacles;
      *simulation_frame_data_.mutable_traffic_light() = traffic_light;
    }

    system_time += std::chrono::microseconds(
        static_cast<int64_t>(10000.0 / playback_status().playback_speed_x));
    std::this_thread::sleep_until(system_time);
  }
}

interface::simulation::SimulationSystemData ReplaySystem::FetchData() {
  interface::simulation::SimulationSystemData result;
  {
    utils::MutexLocker lock(&mutex_);
    result.CopyFrom(simulation_frame_data_);
    if (!playback_status().playback_paused) {
      played_time_ += 0.1;
    }
  }
  return result;
}
}

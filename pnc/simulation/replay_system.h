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

  explicit ReplaySystem(const std::string& record_filename) {
    record_filename_ = record_filename;
    index_filename_ = record_filename + ".index";
    if (file::path::Exists(index_filename_)) {
      LOG(ERROR) << "Index file:" << index_filename_ << " exists, seek functionality is enabled";
      index_ = std::make_unique<utils::IndexReader>(index_filename_);
    }
  }

  void Initialize();

  void Start();

  interface::simulation::SimulationSystemData FetchData() override;

 private:
  std::string record_filename_;
  std::string index_filename_;
  std::unique_ptr<utils::IndexReader> index_;

  utils::Mutex mutex_{"replay"};

  interface::simulation::SimulationSystemData simulation_frame_data_ GUARDED_BY(mutex_);
  std::unique_ptr<file::FileInterface> simulation_log_file_;
};

};  // namespace simulation
